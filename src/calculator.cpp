#include "calculator.h"

#include <QDebug>

using namespace std;

Calculator::Calculator(const RawChip *cur):
    curChip(cur)
{
    calcData();
}

void Calculator::setCurChip(const RawChip *cur)
{
    curChip = cur;
    calcData();
}

const std::vector<double> &Calculator::getV() const
{
    return data;
}

const std::vector<double> &Calculator::getC() const
{
    //qDebug() << con;
    return con;
}

void Calculator::calcData()
{
    //qDebug() << "calc data " << curChip;
    m = curChip->n*curChip->n + Chip::inputPipe + Chip::outputPipe;
    data.resize(curChip->width.size());
    if (!getPara(input, output, R, del))
    {
        data.assign(curChip->width.size(), 0);
        con.assign(curChip->width.size(), 0);
        qDebug() << "not connective!";
        return;
    }
    //qDebug() << data;
    getPhi(m, R,
           Chip::inputPipe, input, Chip::outputPipe, output,
           del, phi);
    /*for (int i=0; i<m; i++)
        qDebug() << R[i];*/
    for (int i=0; i<curChip->width.size(); i++)
    {
        if (!curChip->width[i])
            continue;
        if (curChip->isVertical(i))
        {
            int x = curChip->X(i), y = curChip->Y(i);
            int u = mID(x,y), d = mID(x,y+1);
            data[i] = phi[u]-phi[d];
        }
        else
        {
            int x = curChip->X(i), y = curChip->Y(i);
            int l = mID(x,y), r = mID(x+1,y);
            data[i] = phi[l]-phi[r];
        }
        data[i]*=40000;
        if (fabs(data[i]) < eps)
            data[i] = 0;
    }

    getConcentration();
    /*QString debug;
    qDebug() << "phi: ";
    for (int j=0; j<=curChip->n+1; j++)
    {
        debug = "";
        for (int i=0; i<curChip->n; i++)
            QTextStream(&debug) << QString("%1 ").arg(
                                       mID(i,j)==-1? 100000: phi[mID(i, j)]*1000);
        qDebug() << debug;
    }
    qDebug() << "data: " << data;*/
    //qDebug() << "con: " << con;

}

void Calculator::getPhi(int n, const vector<vector<double> > &R,
                            int ni, int input[], int no, int output[],
                            const std::vector<bool> &del, std::vector<double> &phi)
{
    vector<vector<double> > a;
    a.resize(n);
    for (int i=0; i<n; i++)
        a[i].assign(n+1, 0);
    vector<bool> vis;
    vis.assign(n, 0);
    phi.assign(n, 0);
    //memset(phi, 0, sizeof(phi));
    for (int i=0; i<ni; i++)
    {
        a[input[i]][n] = 1;
    }
    for (int i=0; i<no; i++)
    {
        vis[output[i]] = 1;
        a[output[i]][output[i]] = 1;
    }
    for (int i=0; i<n; i++)
    {
        if (vis[i])
            continue;
        for (int j=0; j<n; j++)
            if (R[i][j])
        {
            a[i][j] += 1.0/R[i][j];
            a[i][i] -= 1.0/R[i][j];
        }
    }
    for (int i=0; i<n; i++)
    {
        if (del[i])
            continue;
        int k=i;
        double mx=fabs(a[i][i]);
        for (int j=i+1; j<n; j++)
            if (!del[j] && fabs(a[j][i])>mx)
            {
                mx=fabs(a[j][i]);
                k=j;
            }
        if (i!=k)
            for (int j=0; j<=n; j++)
                    swap(a[i][j],a[k][j]);
        for (int j=i+1; j<n; j++)
        {
            if (del[j])
                continue;
            double p=a[j][i]/a[i][i];
            for (int k=0; k<=n; k++)
              a[j][k]-=p*a[i][k];
        }
    }
    for (int i=n-1; i>=0; i--)
    {
        if (del[i])
        {
            phi[i] = -1;
            continue;
        }
        phi[i] = a[i][n]/a[i][i];
        for (int j=0; j<=i; j++) a[j][n]-=phi[i]*a[j][i];
    }
}

int Calculator::mID(int i, int j)
{
    int nn = curChip->n*curChip->n;
    if (i<0 || j<0 || i>=curChip->n || j>curChip->n+1)
        return -1;
    if (j == 0)
    {
        for (int id=0; id<Chip::inputPipe; id++)
            if (curChip->input[id] == i)
            {
                //qDebug("mID %d %d : %d", i, j, nn+id);
                return nn+id;
            }
        return -1;
    }
    if (j == curChip->n+1)
    {
        for (int id=0; id<Chip::outputPipe; id++)
            if (curChip->output[id] == i)
                return nn + Chip::inputPipe + id;
        return -1;
    }
    return i*curChip->n+j-1;
}

int findFa(vector<int>& fa, int x) { return fa[x]==x ? x : fa[x]=findFa(fa, fa[x]);}

bool Calculator::getPara(int input[], int output[],
                             vector<vector<double> > &R, vector<bool> &del)
{
    int n = curChip->n;
    int nn = n*n;
    // get input and output
    for (int i=0; i<Chip::inputPipe; i++)
        input[i] = nn+i;
    for (int i=0; i<Chip::outputPipe; i++)
        output[i] = nn+Chip::inputPipe+i;
    // get R
    R.resize(m);
    for (int i=0; i<m; i++)
        R[i].assign(m, 0);
    vector<int> fa;
    fa.resize(m);
    for (int i=0; i<m; i++)
        fa[i] = i;
    auto paraUpdate = [&] (int f, int t, int wid)
    {
        if (t<0 || t>=m)
            return;
        if (!curChip->validID(wid) || !curChip->width[wid])
            return;
        //qDebug("para %d %d %d\n", f, t, wid);
        R[f][t] = R[t][f] = 1.0/curChip->width[wid];
        int f1 = findFa(fa, f), f2 = findFa(fa, t);
        if (f1 != f2)
            fa[f1] = f2;
    };
    for (int i=0; i<n; i++)
        for (int j=1; j<=n; j++)
        {
            int id = mID(i,j);
            paraUpdate(id, mID(i-1,j), curChip->leftID(i,j));
            paraUpdate(id, mID(i+1,j), curChip->rightID(i,j));
            paraUpdate(id, mID(i,j-1), curChip->upID(i,j));
            paraUpdate(id, mID(i,j+1), curChip->downID(i,j));
        }
    for (int i=0; i<m; i++)
        fa[i] = findFa(fa, i);
    // get del
    del.resize(m);
    //QVector<int> vec = curChip->getConnectivity();
    //qDebug() << "connect " << fa;
    int stdBlock = fa[input[0]];
    for (int i=0; i<Chip::inputPipe; i++)
        if (fa[input[i]] != stdBlock)
        {
            //qDebug("wa in %d %d", i, input[i]);
            return 0;
        }
    for (int i=0; i<Chip::outputPipe; i++)
        if (fa[output[i]] != stdBlock)
        {
            //qDebug("wa out %d %d", i, output[i]);
            return 0;
        }
    //memset(del, 0, sizeof(del));
    for (int i=0; i<m; i++)
        if (fa[i] != stdBlock)
            del[i] = 1;
        else
            del[i] = 0;
    return 1;
}

void Calculator::getConcentration()
{
    /*
    vector<double> flow = data;
    for (int i=0; i<flow.size(); i++)
        flow[i]*=curChip->width[i];
    */
    con.assign(data.size(), 0);
    con[curChip->getID(1, curChip->input[0], 0)] = 1;
    con[curChip->getID(1, curChip->input[1], 0)] = 0;
    vector<int> du;
    du.assign(m, 0);
    for (int i=0; i<curChip->n; i++)
        for (int j=1; j<=curChip->n; j++)
        {
            int id = mID(i, j);
            if (del[id]) continue;
            int uid = curChip->upID(i, j);
            if (curChip->validID(uid) && data[uid]<0)
                du[id]++;
            int did = curChip->downID(i, j);
            if (curChip->validID(did) && data[did]>0)
                du[id]++;
            int lid = curChip->leftID(i, j);
            if (curChip->validID(lid) && data[lid]<0)
                du[id]++;
            int rid = curChip->rightID(i, j);
            if (curChip->validID(rid) && data[rid]>0)
                du[id]++;
            //qDebug("(%d %d) : %d %d %d %d %d\n", i, j, id, uid, did, lid, rid);
        }
    int h=0, w=0;
    vector<int> X;
    X.resize(m+1);
    vector<int> Y;
    Y.resize(m+1);
    //qDebug("input %d %d", input[0], input[1]);
    for (int i=0; i<=1; i++)
    {
        int id = mID(curChip->input[i], 1);
        du[id]--;
        if (!du[id])
        {
            w++;
            X[w] = curChip->input[i];
            Y[w] = 1;
        }
    }
    /*QString debug;
    qDebug() << "du";
    for (int j=1; j<=curChip->n; j++)
    {
        debug = "";
        for (int i=0; i<curChip->n; i++)
            QTextStream(&debug) << QString("%1 ").arg(du[mID(i, j)]);
        qDebug() << debug;
    }*/
    while (h!=w)
    {
        h++;
        int x=X[h], y=Y[h];
        //qDebug("now %d %d", x, y);
        int pipe[4] = { curChip->downID(x, y), curChip->rightID(x, y),
                        curChip->upID(x, y), curChip->leftID(x, y)};
        double in[4], out[4];
        memset(in, 0, sizeof(in));
        memset(out, 0, sizeof(out));
        for (int i=0; i<4; i++)
        {
            if (!curChip->validID(pipe[i]) || !curChip->width[pipe[i]])
                continue;
            if ((i<2) == (data[pipe[i]]>0))
                in[i] = fabs(data[pipe[i]])*curChip->width[pipe[i]];
            else
                out[i] = fabs(data[pipe[i]])*curChip->width[pipe[i]];
        }
        /*QString debug = "in[i]:";
        for (int i=0; i<4; i++)
            QTextStream(&debug) << QString("%1 ").arg(in[i]);
        qDebug() << debug;
        debug = "out[i]:";
        for (int i=0; i<4; i++)
            QTextStream(&debug) << QString("%1 ").arg(out[i]);
        qDebug() << debug;*/
        int nin = 0, nout = 0;
        for (int i=0; i<4; i++)
            nin+=(in[i]>0), nout+=(out[i]>0);
        if (nin == 2 && nout==2 && (in[0]>0)!=(in[2]>0))
        {
            //qDebug("diff %d %d", x, y);
            int p1, p2, p3, p4;
            //qDebug("%lf %lf %lf %lf", in[0], in[1], in[2], in[3]);
            //qDebug("%lf %lf %lf %lf", out[0], out[1], out[2], out[3]);
            for (int i=0; i<4; i++)
                if (in[i]>0)
                {
                    //qDebug("in %d", i);
                    for (int j=1; j<=3; j+=2)
                    {
                        int x=(i+j)%4;
                        //qDebug("out %d: %lf %lf %d %d\n", x, in[i], out[x],
                        //       out[x]>0, in[i] >= out[x]-Calculator::eps);
                        if (out[x]>0 && in[i] >= out[x]-Calculator::eps)
                        {
                            //qDebug("set  %d %d", i, x);
                            p3 = i;
                            p2 = x;
                        }
                    }
                }
            for (int i=0; i<4; i++)
            {
                if (in[i]>0 && i!=p3)
                    p1 = i;
                if (out[i]>0 && i!=p2)
                    p4 = i;
            }
            if (p2<0 || p3<0 || p2>3 || p3>3)
            {
                qDebug("Error %d %d : %d %d %d %d", x, y, p1, p2, p3, p4);
                qDebug("%lf %lf %lf %lf", in[0], in[1], in[2], in[3]);
                qDebug("%lf %lf %lf %lf", out[0], out[1], out[2], out[3]);
            }
            /*qDebug("%d %d %d %d", p1, p2, p3, p4);
            for (int i=0; i<4; i++)
                qDebug() << pipe[i];*/
            con[pipe[p2]] = con[pipe[p3]];
            con[pipe[p4]] = (con[pipe[p1]]*in[p1] + con[pipe[p3]]*in[p3]
                             - con[pipe[p2]]*out[p2])/
                            out[p4];
            //qDebug("hehe");

        }
        else
        {
            double cIn = 0, flowIn = 0;
            for (int i=0; i<4; i++)
                if (in[i]>0)
                    cIn+=in[i]*con[pipe[i]], flowIn+=in[i];
            double c = cIn/flowIn;
            for (int i=0; i<4; i++)
                if (out[i]>0)
                {
                    con[pipe[i]] = c;
                    //qDebug("set %d to %lf", pipe[i], c);
                }
        }
        int dx[4] = {0, 1, 0, -1};
        int dy[4] = {1, 0, -1, 0};
        for (int i=0; i<4; i++)
        {
            if (!(out[i]>0))
                continue;
            int id = mID(x+dx[i], y+dy[i]);
            //qDebug("now %d : %d %d",id, x+dx[i], y+dy[i]);
            du[id]--;
            if (du[id]==0)
            {
                //qDebug("push %d %d", x+dx[i], y+dy[i]);
                w++;
                X[w] = x+dx[i];
                Y[w] = y+dy[i];
            }
        }
        //qDebug() << con;
    }
    for (int i=0; i<data.size(); i++)
        if (curChip->width[i]>0 && data[i] == 0)
            con[i] = 10000;
    /*qDebug() << "queue";
    debug = "";
    for (int i=1; i<=w; i++)
        QTextStream(&debug) << QString("(%1,%2)").arg(X[i]).arg(Y[i]);
    qDebug() << debug;*/
}

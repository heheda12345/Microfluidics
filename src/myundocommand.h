#ifndef MYUNDOCOMMAND_H
#define MYUNDOCOMMAND_H

#include <QUndoCommand>
#include <QDebug>
#include "chip.h"

template<class T, class Value>
class MyUndoCommand : public QUndoCommand
{
public:
    MyUndoCommand(T& target, Value& newItem, QUndoCommand *parent = 0):
        QUndoCommand(parent), m_target(target), m_pre(target.value()), m_cur(newItem) {}
    void undo()
    {
        m_target.setValue(m_pre);
    }
    void redo()
    {
        //qDebug() << m_pre.width;
        //qDebug() << m_cur.width;
        m_target.setValue(m_cur);
    }

signals:

public slots:
private:
    T& m_target;
    Value m_pre, m_cur;
};


#endif // MYUNDOCOMMAND_H

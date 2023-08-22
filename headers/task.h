#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QPixmap>

class Task {
public:
    Task(const QString &description, bool completed = false);
    QString getDescription() const;
    bool isCompleted() const;
    void toggleComplete();
    void setImagePath(const QString &path);
    QString getImagePath() const;
    QPixmap getImage() const;

private:
    QString description;
    bool completed;
    QString imagePath;
};

#endif // TASK_H

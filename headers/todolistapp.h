#ifndef TODOLISTAPP_H
#define TODOLISTAPP_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QLabel>
#include "task.h"

class ToDoListApp : public QMainWindow {
    Q_OBJECT
public:
    ToDoListApp(QWidget *parent = nullptr);

private slots:
    void addTask();
    void toggleTaskComplete(QListWidgetItem *item);
    void saveTasks();
    void loadTasks();
    void addImageToTask();

private:
    void updateTaskList();
    void cacheTasksToFile();
    void cacheTasksFromCacheFile();

    QLineEdit *taskInput;
    QPushButton *addButton;
    QListWidget *taskList;
    QPushButton *saveButton;
    QPushButton *loadButton;
    QPushButton *addImageButton;
    QLabel *imageLabel;
    QVector<Task> tasks;
    QString cacheFilePath;
};

#endif // TODOLISTAPP_H

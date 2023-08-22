#include "../headers/todolistapp.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ToDoListApp::ToDoListApp(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("To-Do List App");

    QVBoxLayout *layout = new QVBoxLayout;

    taskInput = new QLineEdit;
    addButton = new QPushButton("Add Task");
    taskList = new QListWidget;
    saveButton = new QPushButton("Save Tasks");
    loadButton = new QPushButton("Load Tasks");
    addImageButton = new QPushButton("Add Image");
    imageLabel = new QLabel;

    layout->addWidget(taskInput);
    layout->addWidget(addButton);
    layout->addWidget(taskList);
    layout->addWidget(saveButton);
    layout->addWidget(loadButton);
    layout->addWidget(addImageButton);
    layout->addWidget(imageLabel);

    centralWidget()->setLayout(layout);

    connect(addButton, &QPushButton::clicked, this, &ToDoListApp::addTask);
    connect(taskList, &QListWidget::itemDoubleClicked, this, &ToDoListApp::toggleTaskComplete);
    connect(saveButton, &QPushButton::clicked, this, &ToDoListApp::saveTasks);
    connect(loadButton, &QPushButton::clicked, this, &ToDoListApp::loadTasks);
    connect(addImageButton, &QPushButton::clicked, this, &ToDoListApp::addImageToTask);

    // Define the path for the cache file
    cacheFilePath = "cached_tasks.json";

    // Load cached tasks on application startup
    cacheTasksFromCacheFile();
}

void ToDoListApp::addTask() {
    QString taskDescription = taskInput->text();
    if (!taskDescription.isEmpty()) {
        Task task(taskDescription);
        tasks.append(task);
        updateTaskList();
        taskInput->clear();
        cacheTasksToFile();
    }
}

void ToDoListApp::toggleTaskComplete(QListWidgetItem *item) {
    int index = taskList->row(item);
    tasks[index].toggleComplete();
    updateTaskList();
    cacheTasksToFile();
}

void ToDoListApp::saveTasks() {
    QFile file("tasks.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        for (const Task &task : tasks) {
            stream << task.getDescription() << "\t" << (task.isCompleted() ? "1" : "0") << "\t" << task.getImagePath() << "\n";
        }
        file.close();
        QMessageBox::information(this, "Tasks Saved", "Tasks saved to tasks.txt");
    } else {
        QMessageBox::warning(this, "Error", "Could not save tasks to file.");
    }
    cacheTasksToFile();
}

void ToDoListApp::loadTasks() {
    taskList->clear();
    tasks.clear();
    QFile file("tasks.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            QStringList parts = line.split('\t');
            if (parts.size() >= 3) {
                Task task(parts[0], parts[1] == "1");
                task.setImagePath(parts[2]);
                tasks.append(task);
            }
        }
        file.close();
        updateTaskList();
        QMessageBox::information(this, "Tasks Loaded", "Tasks loaded from tasks.txt");
    } else {
        QMessageBox::warning(this, "Error", "Could not load tasks from file.");
    }
    cacheTasksToFile();
}

void ToDoListApp::addImageToTask() {
    QString imagePath = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.jpg *.jpeg)");
    if (!imagePath.isEmpty()) {
        int currentIndex = taskList->currentIndex().row();
        if (currentIndex >= 0 && currentIndex < tasks.size()) {
            tasks[currentIndex].setImagePath(imagePath);
            QPixmap image(imagePath);
            imageLabel->setPixmap(image.scaledToHeight(100));
            cacheTasksToFile();
        }
    }
}

void ToDoListApp::updateTaskList() {
    taskList->clear();
    for (const Task &task : tasks) {
        QListWidgetItem *item = new QListWidgetItem(task.getDescription());
        if (task.isCompleted()) {
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Checked);
        }
        taskList->addItem(item);
    }
}

void ToDoListApp::cacheTasksToFile() {
    QFile cacheFile(cacheFilePath);
    if (cacheFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QJsonArray tasksArray;
        for (const Task &task : tasks) {
            QJsonObject taskObject;
            taskObject["description"] = task.getDescription();
            taskObject["completed"] = task.isCompleted();
            taskObject["imagePath"] = task.getImagePath();
            tasksArray.append(taskObject);
        }

        QJsonDocument jsonDocument(tasksArray);
        QTextStream stream(&cacheFile);
        stream << jsonDocument.toJson(QJsonDocument::Indented);
        cacheFile.close();
    }
}

void ToDoListApp::cacheTasksFromCacheFile() {
    QFile cacheFile(cacheFilePath);
    if (cacheFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray jsonData = cacheFile.readAll();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);
        if (jsonDocument.isArray()) {
            QJsonArray tasksArray = jsonDocument.array();
            tasks.clear();
            for (const QJsonValue &taskValue : tasksArray) {
                if (taskValue.isObject()) {
                    QJsonObject taskObject = taskValue.toObject();
                    QString description = taskObject["description"].toString();
                    bool completed = taskObject["completed"].toBool();
                    QString imagePath = taskObject["imagePath"].toString();
                    Task task(description, completed);
                    task.setImagePath(imagePath);
                    tasks.append(task);
                }
            }
            updateTaskList();
        }
        cacheFile.close();
    }
}

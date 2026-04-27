#include "Program2Window.hpp"
#include "InkObject.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QHeaderView>
#include <QFileInfo>

Program2Window::Program2Window(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget;
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    QHBoxLayout *topLayout = new QHBoxLayout;
    m_selectBtn = new QPushButton("Выбрать JSON файл");
    m_fileLabel = new QLabel("Файл не выбран");
    m_moveBtn = new QPushButton("Перенести битые в отдельный файл");
    m_moveBtn->setEnabled(false);
    topLayout->addWidget(m_selectBtn);
    topLayout->addWidget(m_fileLabel, 1);
    topLayout->addWidget(m_moveBtn);
    mainLayout->addLayout(topLayout);

    QLabel *labelValid = new QLabel("Таблица 1 – Корректные объекты");
    m_tableValid = new QTableWidget;
    m_tableValid->setColumnCount(4);
    m_tableValid->setHorizontalHeaderLabels({"Название", "Описание", "Цвет", "Чернила"});
    m_tableValid->horizontalHeader()->setStretchLastSection(true);

    QLabel *labelBroken = new QLabel("Таблица 2 – Битые объекты");
    m_tableBroken = new QTableWidget;
    m_tableBroken->setColumnCount(4);
    m_tableBroken->setHorizontalHeaderLabels({"Название", "Описание", "Цвет", "Чернила"});
    m_tableBroken->horizontalHeader()->setStretchLastSection(true);

    mainLayout->addWidget(labelValid);
    mainLayout->addWidget(m_tableValid);
    mainLayout->addWidget(labelBroken);
    mainLayout->addWidget(m_tableBroken);

    connect(m_selectBtn, &QPushButton::clicked, this, &Program2Window::onSelectFile);
    connect(m_moveBtn, &QPushButton::clicked, this, &Program2Window::onMoveBrokenToFile);

    setWindowTitle("Программа 2 – Анализ JSON");
    resize(900, 600);
}

void Program2Window::onSelectFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите JSON файл", "", "JSON files (*.json)");
    if (fileName.isEmpty()) return;
    m_currentFilePath = fileName;
    m_fileLabel->setText(QFileInfo(fileName).fileName());
    loadFromFile(fileName);
    m_moveBtn->setEnabled(!m_brokenObjects.isEmpty());
}

void Program2Window::loadFromFile(const QString &fileName)
{
    m_validObjects.clear();
    m_brokenObjects.clear();

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл.");
        updateTables();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (!doc.isArray()) {
        QMessageBox::warning(this, "Предупреждение", "Файл не содержит JSON массив.");
        updateTables();
        return;
    }

    QJsonArray arr = doc.array();
    for (const QJsonValue &val : arr) {
        if (!val.isObject()) continue;
        InkObject obj;
        obj.fromJson(val.toObject());
        if (obj.isValid())
            m_validObjects.append(obj);
        else
            m_brokenObjects.append(obj);
    }

    updateTables();
}

void Program2Window::updateTables()
{
    m_tableValid->setRowCount(m_validObjects.size());
    for (int i = 0; i < m_validObjects.size(); ++i) {
        const InkObject &obj = m_validObjects[i];
        m_tableValid->setItem(i, 0, new QTableWidgetItem(obj.getName()));
        m_tableValid->setItem(i, 1, new QTableWidgetItem(obj.getDescription()));
        m_tableValid->setItem(i, 2, new QTableWidgetItem(obj.getColor()));
        m_tableValid->setItem(i, 3, new QTableWidgetItem(QString::number(obj.getInkLevel())));
    }

    m_tableBroken->setRowCount(m_brokenObjects.size());
    for (int i = 0; i < m_brokenObjects.size(); ++i) {
        const InkObject &obj = m_brokenObjects[i];
        m_tableBroken->setItem(i, 0, new QTableWidgetItem(obj.getName().isEmpty() ? "<пусто>" : obj.getName()));
        m_tableBroken->setItem(i, 1, new QTableWidgetItem(obj.getDescription().isEmpty() ? "<пусто>" : obj.getDescription()));
        m_tableBroken->setItem(i, 2, new QTableWidgetItem(obj.getColor().isEmpty() ? "<пусто>" : obj.getColor()));
        QString inkStr = (obj.getInkLevel() < 0) ? "<некорр>" : QString::number(obj.getInkLevel());
        m_tableBroken->setItem(i, 3, new QTableWidgetItem(inkStr));
    }
}

void Program2Window::onMoveBrokenToFile()
{
    if (m_brokenObjects.isEmpty()) {
        QMessageBox::information(this, "Перенос", "Нет битых объектов для переноса.");
        return;
    }

    QString saveFileName = QFileDialog::getSaveFileName(this, "Сохранить битые объекты", "broken.json", "JSON files (*.json)");
    if (saveFileName.isEmpty()) return;

    QJsonArray arr;
    for (const InkObject &obj : m_brokenObjects)
        arr.append(obj.toJson());

    QFile file(saveFileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать файл для битых объектов.");
        return;
    }
    QJsonDocument doc(arr);
    file.write(doc.toJson());
    file.close();

    QMessageBox::information(this, "Успех", QString("Битые объекты (%1 шт.) сохранены в %2").arg(m_brokenObjects.size()).arg(saveFileName));

    m_brokenObjects.clear();
    updateTables();
    m_moveBtn->setEnabled(false);
}

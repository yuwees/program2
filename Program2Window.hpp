#ifndef PROGRAM2WINDOW_HPP
#define PROGRAM2WINDOW_HPP

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>

class Program2Window : public QMainWindow
{
    Q_OBJECT
public:
    explicit Program2Window(QWidget *parent = nullptr);

private slots:
    void onSelectFile();
    void onMoveBrokenToFile();

private:
    void loadFromFile(const QString &fileName);
    void updateTables();

    QPushButton *m_selectBtn;
    QPushButton *m_moveBtn;
    QLabel      *m_fileLabel;
    QTableWidget *m_tableValid;
    QTableWidget *m_tableBroken;

    QString m_currentFilePath;
    QList<InkObject> m_validObjects;
    QList<InkObject> m_brokenObjects;
};

#endif // PROGRAM2WINDOW_HPP

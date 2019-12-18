#ifndef PASSWORD_H
#define PASSWORD_H

#include <QWidget>

class QLineEdit;

class PasswordWindow : public QWidget
{
    Q_OBJECT
public:
    PasswordWindow(QWidget* parent = nullptr, const QString& userName = QString());

private slots:
//    void changePassword();

private:
    QLineEdit* m_oldPwd;
    QLineEdit* m_newPwd;
    QString m_name;

    void drawInterface(const QString& userName);
};

#endif  // PASSWORD_H

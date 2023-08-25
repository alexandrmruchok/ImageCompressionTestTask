#include "qobject.h"
#include <QTest>

class BMPImageTests: public QObject
{
    Q_OBJECT

private slots:
    void toUpper();

signals:

};

void BMPImageTests::toUpper()
{
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}

QTEST_MAIN(BMPImageTests)
#include "BMPImageTests.moc"

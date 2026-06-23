#ifndef CIDRCALCULATOR_H
#define CIDRCALCULATOR_H

#include <QMainWindow>
#include <QLabel>
#include <QTextEdit>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QClipboard>
#include <QApplication>
#include <QGroupBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDebug>
#include <vector>

#include "ldettabwidget.h"
#include "ldetlineedit.h"
#include "ldetpushbtn.h"

class CIDRCalculator : public QMainWindow
{
    Q_OBJECT

public:
    CIDRCalculator(QWidget *parent = nullptr);
    ~CIDRCalculator();

private slots:
    void calculateFromCIDR();
    void calculateFromRange();
    void copyCIDRResults();
    void validateIPInput();
    void validateCIDRInput();

private:
    // 创建UI组件
    void setupUI();

    // CIDR计算相关方法
    bool isValidCIDR(const QString &cidr);
    bool isValidIP(const QString &ip);
    quint32 ipToInt(const QString &ip);
    QString intToIP(quint32 ip);
    void calculateCIDRRange(const QString &cidr);

    // IP范围计算相关方法
    bool isValidIPRange(const QString &startIP, const QString &endIP);
    QString rangeToCIDR(const QString &startIP, const QString &endIP);
    quint32 getNetworkAddress(quint32 ip, int prefix);
    quint32 getBroadcastAddress(quint32 ip, int prefix);

    // UI组件
    LDETTabWidget *tabWidget;

    // CIDR到范围页面
    QWidget *cidrToRangeTab;
    LDETLineEdit *cidrInput;
    LDETPushBtn *cidrCalculateBtn;
    LDETLineEdit *startIPOutput;
    LDETLineEdit *endIPOutput;
    LDETLineEdit *totalIPsOutput;

    // 范围到CIDR页面
    QWidget *rangeToCIDRTab;
    LDETLineEdit *rangeStartInput;
    LDETLineEdit *rangeEndInput;
    LDETPushBtn *rangeCalculateBtn;
    LDETLineEdit *cidrResultsOutput;
    LDETPushBtn *copyCIDRBtn;
};

#endif // CIDRCALCULATOR_H
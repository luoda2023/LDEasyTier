#ifndef LDEASYTIER_LDETSERVERSDIALOG_H
#define LDEASYTIER_LDETSERVERSDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include "ldetcheckbtn.h"
#include "ldetpushbtn.h"

struct ServerInfoData
{
    QString name;
    QString address;
    bool isPublic = false;

    ServerInfoData() = default;
    ServerInfoData(QString n, QString a, bool pub = false)
        : name(std::move(n)), address(std::move(a)), isPublic(pub) {}

    static ServerInfoData fromJson(const QJsonObject &obj) {
        ServerInfoData info;
        // 兼容用户收藏格式 { "name": "...", "address": "..." }
        if (obj.contains("name") && obj.contains("address")) {
            info.name = obj["name"].toString();
            info.address = obj["address"].toString();
            info.isPublic = false;
        }
        // 兼容公共服务器格式 { "contributor": "...", "url": "..." }
        else if (obj.contains("contributor") && obj.contains("url")) {
            info.name = obj["contributor"].toString();
            info.address = obj["url"].toString();
            info.isPublic = true;
        }
        return info;
    }
};

class LDETServersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LDETServersDialog(QWidget *parent = nullptr);
    ~LDETServersDialog() override = default;

    void setSelectedServers(const QStringList &selectedAddresses);
    [[nodiscard]] QStringList selectedServers() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void initUI();
    void loadServers();
    void loadPublicServers();
    void updateServerList();
    [[nodiscard]] QString serverFilePath() const;
    [[nodiscard]] QString publicServerFilePath() const;
    void updateColorScheme();
    void clearServerCheckboxes();

    QVBoxLayout *m_mainLayout = nullptr;
    QLabel *m_titleLabel = nullptr;
    QScrollArea *m_scrollArea = nullptr;
    QWidget *m_scrollContent = nullptr;
    QVBoxLayout *m_scrollLayout = nullptr;

    LDETPushBtn *m_selectAllBtn = nullptr;
    LDETPushBtn *m_deselectAllBtn = nullptr;
    LDETPushBtn *m_okBtn = nullptr;
    LDETPushBtn *m_cancelBtn = nullptr;

    QVector<ServerInfoData> m_servers;
    QList<LDETCheckBtn*> m_serverCheckBoxes;

    QStringList m_initiallySelectedAddresses;

    QColor m_bgColor;
    QColor m_borderColor;

private slots:
    void onSelectAll();
    void onDeselectAll();
};

#endif // LDEASYTIER_LDETSERVERSDIALOG_H

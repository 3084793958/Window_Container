#ifndef EASY_DESKTOP_CLASS_H
#define EASY_DESKTOP_CLASS_H
#include <QString>
#include <QVariant>

struct P_Version
{
    uint Major_Version;
    uint Minor_Version;
    uint Patch_Version;
    P_Version(uint m_Major_Version, uint m_Minor_Version, uint m_Patch_Version);
    P_Version(QString Version_Name);
    bool operator==(P_Version m_version) const;
    bool operator!=(P_Version m_version) const;
    bool operator>(P_Version m_version) const;
    bool operator>=(P_Version m_version) const;
    bool operator<(P_Version m_version) const;
    bool operator<=(P_Version m_version) const;
    QString VersionName() const;
};
class P_Sender : public QObject
{
    Q_OBJECT
public:
    explicit P_Sender(QObject *parent = nullptr, bool m_allow_Send_Data = true, bool m_allow_Send_Ptr = true);
    void Send();
    void Send_Data(QVariant var);
    void Send_Ptr(void *ptr);
    bool allow_Send_Data = true;
    bool allow_Send_Ptr = true;
signals:
    void sig_Send();
    void sig_Send_Data(QVariant var);
    void sig_Send_Ptr(void *ptr);
};

#endif // EASY_DESKTOP_CLASS_H

#ifndef RECENTREQUESTSFILTERPROXY_H
#define RECENTREQUESTSFILTERPROXY_H

#include <QObject>
#include <QSortFilterProxyModel>
#include <QImage>

class SendCoinsRecipient;

class RecentRequestsFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT


public:
    explicit RecentRequestsFilterProxy(QObject *parent = nullptr);

    Q_INVOKABLE void sortColumn(QString roleName, Qt::SortOrder order);
    Q_INVOKABLE QVariant getData(QString roleName, int sourceRow);
    Q_INVOKABLE void newRequest(const QString &label, QString address, const QString &message, const QString &amountStr,int unit);
    Q_INVOKABLE void saveImage();
    Q_INVOKABLE void copyImage();
    Q_INVOKABLE void copyURI();
    Q_INVOKABLE void copyAddress();
    Q_INVOKABLE void showAddress(int row);
    Q_INVOKABLE void removeAddress(QVariantList rows);


signals:
    void sendCoinsRecipientReady(QString info,QString address,QString label);

private:
    void prepareQRCode();
    QImage m_img;
    QString html;

    SendCoinsRecipient *m_info;
};

#endif // RECENTREQUESTSFILTERPROXY_H

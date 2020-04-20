#include "recentrequestsfilterproxy.h"
#include "recentrequeststablemodel.h"
#include "bitcoinunits.h"
#include "addresstablemodel.h"
#include "optionsmodel.h"
#include "walletmodel.h"
#include "guiutil.h"
#include <qrencode.h>

#include "imageprovider.h"

extern ImageProvider *imageProvider;


RecentRequestsFilterProxy::RecentRequestsFilterProxy(QObject *parent):QSortFilterProxyModel(parent)
{
        m_info = new SendCoinsRecipient();
}


void RecentRequestsFilterProxy::sortColumn(QString roleName, Qt::SortOrder order)
{
    RecentRequestsTableModel::ColumnIndex ci;

    if(roleName == "date")
        ci = RecentRequestsTableModel::Date;
    else if(roleName == "label")
        ci = RecentRequestsTableModel::Label;
    else if(roleName == "message")
        ci = RecentRequestsTableModel::Message;
    else if(roleName == "amount")
        ci = RecentRequestsTableModel::Amount;

    sort(ci,order);
}


QVariant RecentRequestsFilterProxy::getData(QString roleName, int sourceRow)
{
    if(roleName == "date"){
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(RecentRequestsTableModel::DateRole).toString();
    }
    else if(roleName == "label")
    {
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(RecentRequestsTableModel::LabelRole).toString();
    }
    else if(roleName == "label_")
    {
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(RecentRequestsTableModel::LabelRole_).toString();
    }
    else if(roleName == "message")
    {
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(RecentRequestsTableModel::MessageRole).toString();
    }
    else if(roleName == "message_")
    {
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(RecentRequestsTableModel::MessageRole_).toString();
    }
    else if(roleName == "amount")
    {
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(RecentRequestsTableModel::AmountRole).toString();
    }
    else if(roleName == "amount_")
    {
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(RecentRequestsTableModel::AmountRole_).toLongLong();
    }
    else if(roleName == "address")
    {
        QModelIndex index_ = mapToSource(this->index(sourceRow,0));
        return index_.data(RecentRequestsTableModel::AddressRole).toString();
    }
}



void RecentRequestsFilterProxy::newRequest(const QString &label, QString address,const QString &message,const QString &amountStr,int unit)
{
    RecentRequestsTableModel  *sourceModel_ = static_cast<RecentRequestsTableModel *> (sourceModel());

    WalletModel *walletModel = sourceModel_->walletModel;

    if(address == "")
        address = walletModel->getAddressTableModel()->addRow(AddressTableModel::Receive, "", "");

    CAmount amount = 0;
	printf("unit:%d\n",unit);
    BitcoinUnits::parse(unit, amountStr, &amount);
	
	printf("amount:%lu\n",amount);
	
    m_info->address = address;
    m_info->label = label;
    m_info->amount = amount;
    m_info->message = message;

    sourceModel_->addNewRequest(*m_info);

    prepareQRCode();

    /* Store request for later reference */
}


void RecentRequestsFilterProxy::prepareQRCode()
{
    QString uri = GUIUtil::formatBitcoinURI(*m_info);
    RecentRequestsTableModel  *sourceModel_ = static_cast<RecentRequestsTableModel *> (sourceModel());

    WalletModel *walletModel = sourceModel_->walletModel;
    html = "";
    html += "<html><font face='verdana, arial, helvetica, sans-serif'>";
    html += "<b>" + tr("Payment information") + "</b><br>";
    html += "<b>" + tr("URI") + "</b>: ";
    html += "<a style=\"color:#12546a;\" href=\"" + uri + "\">" + GUIUtil::HtmlEscape(uri) + "</a><br>";
    html += "<b>" + tr("Address") + "</b>: " + GUIUtil::HtmlEscape(m_info->address) + "<br>";
    if (m_info->amount)
        html += "<b>" + tr("Amount") + "</b>: " + BitcoinUnits::formatWithUnit(walletModel->getOptionsModel()->getDisplayUnit(), m_info->amount) + "<br>";
    if (!m_info->label.isEmpty())
        html += "<b>" + tr("Label") + "</b>: " + GUIUtil::HtmlEscape(m_info->label) + "<br>";
    if (!m_info->message.isEmpty())
        html += "<b>" + tr("Message") + "</b>: " + GUIUtil::HtmlEscape(m_info->message) + "<br>";


    if (!uri.isEmpty())
    {
        QRcode* code = QRcode_encodeString(uri.toUtf8().constData(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);
        if (!code) {
            return;
        }
        QImage myImage = QImage(code->width + 8, code->width + 8, QImage::Format_RGB32);
        myImage.fill(0xffffff);
        unsigned char* p = code->data;
        for (int y = 0; y < code->width; y++) {
            for (int x = 0; x < code->width; x++) {
                myImage.setPixel(x + 4, y + 4, ((*p & 1) ? 0x0 : 0xffffff));
                p++;
            }
        }
        QRcode_free(code);

        myImage = myImage.scaled(300,300);

        imageProvider->setImage(myImage);
        m_img = myImage;
    }

    emit sendCoinsRecipientReady(html,m_info->address,m_info->label);

}

void RecentRequestsFilterProxy::copyAddress()
{
    RecentRequestsTableModel  *sourceModel_ = static_cast<RecentRequestsTableModel *> (sourceModel());
    WalletModel *walletModel = sourceModel_->walletModel;
    walletModel->setClipBoard(m_info->address);
}


void RecentRequestsFilterProxy::copyURI()
{
    RecentRequestsTableModel  *sourceModel_ = static_cast<RecentRequestsTableModel *> (sourceModel());
    WalletModel *walletModel = sourceModel_->walletModel;
    walletModel->setClipBoard(GUIUtil::formatBitcoinURI(*m_info));
}


void RecentRequestsFilterProxy::copyImage()
{
    RecentRequestsTableModel  *sourceModel_ = static_cast<RecentRequestsTableModel *> (sourceModel());
    WalletModel *walletModel = sourceModel_->walletModel;
    walletModel->setClipBoard(m_img);
}

void RecentRequestsFilterProxy::saveImage()
{
    QString fn = GUIUtil::getSaveFileName(NULL, tr("Save QR Code"), QString(), tr("PNG Image (*.png)"), NULL);
    if (!fn.isEmpty()) {
        m_img.save(fn);
    }
}

void RecentRequestsFilterProxy::showAddress(int row)
{
    m_info->address = getData("address",row).toString();
    m_info->label = getData("label_",row).toString();
    m_info->amount = getData("amount_",row).toLongLong();
    m_info->message = getData("message_",row).toString();

    prepareQRCode();
}

void RecentRequestsFilterProxy::removeAddress(QVariantList rows)
{
    removeRows(rows.at(0).toInt(),rows.size());
}


#include "masternodemanager.h"
#include "ui_masternodemanager.h"
#include "addeditmastertoad.h"
#include "mastertoadconfigdialog.h"

#include "sync.h"
#include "clientmodel.h"
#include "walletmodel.h"
#include "activemasternode.h"
#include "masternodeconfig.h"
#include "masternode.h"
#include "walletdb.h"
#include "wallet.h"
#include "init.h"

#include <QAbstractItemDelegate>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QScrollArea>
#include <QScroller>
#include <QDateTime>
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>

MasternodeManager::MasternodeManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MasternodeManager),
    clientModel(0),
    walletModel(0)
{
    ui->setupUi(this);

    ui->editButton->setEnabled(false);
    ui->getConfigButton->setEnabled(false);
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(false);
    ui->copyAddressButton->setEnabled(false);
    ui->removeButton->setEnabled(false);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    subscribeToCoreSignals();

    timer = new QTimer(this);
    //connect(timer, SIGNAL(timeout()), this, SLOT(updateNodeList()));
    //connect(timer, SIGNAL(timeout()), this, SLOT(updateMyNodeList()));
    //timer->start(10000);

    
    updateNodeList();
    updateMyNodeList();
}

MasternodeManager::~MasternodeManager()
{
    delete ui;
}

static void NotifymastertoadUpdated(MasternodeManager *page, CmastertoadConfig nodeConfig)
{
    // alias, address, privkey, collateral address
    QString alias = QString::fromStdString(nodeConfig.sAlias);
    QString addr = QString::fromStdString(nodeConfig.sAddress);
    QString privkey = QString::fromStdString(nodeConfig.sMasternodePrivKey);
    QString collateral = QString::fromStdString(nodeConfig.sCollateralAddress);
    
    QMetaObject::invokeMethod(page, "updatemastertoad", Qt::QueuedConnection,
                              Q_ARG(QString, alias),
                              Q_ARG(QString, addr),
                              Q_ARG(QString, privkey),
                              Q_ARG(QString, collateral)
                              );
}

void MasternodeManager::subscribeToCoreSignals()
{
    // Connect signals to core
    uiInterface.NotifymastertoadChanged.connect(boost::bind(&NotifymastertoadUpdated, this, _1));
}

void MasternodeManager::unsubscribeFromCoreSignals()
{
    // Disconnect signals from core
    uiInterface.NotifymastertoadChanged.disconnect(boost::bind(&NotifymastertoadUpdated, this, _1));
}

void MasternodeManager::on_tableWidget_2_itemSelectionChanged()
{
    if(ui->tableWidget_2->selectedItems().count() > 0)
    {
        ui->editButton->setEnabled(true);
        ui->getConfigButton->setEnabled(true);
        ui->startButton->setEnabled(true);
        ui->stopButton->setEnabled(true);
	    ui->copyAddressButton->setEnabled(true);
        ui->removeButton->setEnabled(true);
    }
}

void MasternodeManager::updatemastertoad(QString alias, QString addr, QString privkey, QString collateral, QString status)
{
    LOCK(cs_mastertoad);
    bool bFound = false;
    int nodeRow = 0;
    for(int i=0; i < ui->tableWidget_2->rowCount(); i++)
    {
        if(ui->tableWidget_2->item(i, 0)->text() == alias)
        {
            bFound = true;
            nodeRow = i;
            break;
        }
    }

    if(nodeRow == 0 && !bFound)
        ui->tableWidget_2->insertRow(0);

    QTableWidgetItem *aliasItem = new QTableWidgetItem(alias);
    QTableWidgetItem *addrItem = new QTableWidgetItem(addr);
    QTableWidgetItem *statusItem = new QTableWidgetItem(status);
    QTableWidgetItem *collateralItem = new QTableWidgetItem(collateral);

    ui->tableWidget_2->setItem(nodeRow, 0, aliasItem);
    ui->tableWidget_2->setItem(nodeRow, 1, addrItem);
    ui->tableWidget_2->setItem(nodeRow, 2, statusItem);
    ui->tableWidget_2->setItem(nodeRow, 3, collateralItem);
}

static QString seconds_to_DHMS(quint32 duration)
{
  QString res;
  int seconds = (int) (duration % 60);
  duration /= 60;
  int minutes = (int) (duration % 60);
  duration /= 60;
  int hours = (int) (duration % 24);
  int days = (int) (duration / 24);
  if((hours == 0)&&(days == 0))
      return res.sprintf("%02dm:%02ds", minutes, seconds);
  if (days == 0)
      return res.sprintf("%02dh:%02dm:%02ds", hours, minutes, seconds);
  return res.sprintf("%dd %02dh:%02dm:%02ds", days, hours, minutes, seconds);
}

void MasternodeManager::updateNodeList()
{
    TRY_LOCK(cs_masternodes, lockMasternodes);
    if(!lockMasternodes)
        return;

    ui->countLabel->setText("Updating...");
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    BOOST_FOREACH(CMasterNode mn, vecMasternodes) 
    {
        int mnRow = 0;
        ui->tableWidget->insertRow(0);

 	// populate list
	// Address, Rank, Active, Active Seconds, Last Seen, Pub Key
	QTableWidgetItem *activeItem = new QTableWidgetItem(QString::fromStdString(mn.GetStatus()));
	QTableWidgetItem *addressItem = new QTableWidgetItem(QString::fromStdString(mn.addr.ToString()));
	QTableWidgetItem *rankItem = new QTableWidgetItem(QString::number(GetMasternodeRank(mn.vin, pindexBest->nHeight)));
	QTableWidgetItem *activeSecondsItem = new QTableWidgetItem(seconds_to_DHMS((qint64)(mn.lastTimeSeen - mn.now)));
	QTableWidgetItem *lastSeenItem = new QTableWidgetItem(QString::fromStdString(DateTimeStrFormat(mn.lastTimeSeen)));
	
	CScript pubkey;
        pubkey =GetScriptForDestination(mn.pubkey.GetID());
        CTxDestination address1;
        ExtractDestination(pubkey, address1);
        CBitcoinAddress address2(address1);
	QTableWidgetItem *pubkeyItem = new QTableWidgetItem(QString::fromStdString(address2.ToString()));
	
	ui->tableWidget->setItem(mnRow, 0, addressItem);
	ui->tableWidget->setItem(mnRow, 1, rankItem);
	ui->tableWidget->setItem(mnRow, 2, activeItem);
	ui->tableWidget->setItem(mnRow, 3, activeSecondsItem);
	ui->tableWidget->setItem(mnRow, 4, lastSeenItem);
	ui->tableWidget->setItem(mnRow, 5, pubkeyItem);
    }

    ui->countLabel->setText(QString::number(ui->tableWidget->rowCount()));
}

void MasternodeManager::updateMyNodeList()
{
    TRY_LOCK(cs_mymnlist, fLockAcquired);
    if(!fLockAcquired) {
        return;
    }
    static int64_t nTimeMyListUpdated = 0;
       
    if(pwalletMain)
    {
        BOOST_FOREACH(PAIRTYPE(std::string, CmastertoadConfig) mastertoad, pwalletMain->mapMymastertoads)
        {            
            // try to find status
            std::string sStatus = "";
            TRY_LOCK(cs_masternodes, lockMasternodes);
            if(lockMasternodes)
            {
                BOOST_FOREACH(CMasterNode mn, vecMasternodes) 
                {
                    if(mn.addr.ToString() == mastertoad.second.sAddress)
                    {
                        sStatus = mn.GetStatus();                
                        break;
                    }
                }
            }
            updatemastertoad(QString::fromStdString(mastertoad.second.sAlias), QString::fromStdString(mastertoad.second.sAddress), QString::fromStdString(mastertoad.second.sMasternodePrivKey), QString::fromStdString(mastertoad.second.sCollateralAddress), QString::fromStdString(sStatus));
        }
    }
}


void MasternodeManager::setClientModel(ClientModel *model)
{
    this->clientModel = model;
    if(model)
    {
    }
}

void MasternodeManager::setWalletModel(WalletModel *model)
{
    this->walletModel = model;
    if(model && model->getOptionsModel())
    {
    }

}

void MasternodeManager::on_refreshToadsButton_clicked()
{
    updateNodeList();
}

void MasternodeManager::on_refreshMyToadsButton_clicked()
{
    updateMyNodeList();
}

void MasternodeManager::on_createButton_clicked()
{
    AddEditmastertoad* aenode = new AddEditmastertoad();
    aenode->exec();
}

void MasternodeManager::on_copyAddressButton_clicked()
{
    QItemSelectionModel* selectionModel = ui->tableWidget_2->selectionModel();
    QModelIndexList selected = selectionModel->selectedRows();
    if(selected.count() == 0)
        return;

    QModelIndex index = selected.at(0);
    int r = index.row();
    std::string sCollateralAddress = ui->tableWidget_2->item(r, 3)->text().toStdString();

    QApplication::clipboard()->setText(QString::fromStdString(sCollateralAddress));
}

void MasternodeManager::on_editButton_clicked()
{
    QItemSelectionModel* selectionModel = ui->tableWidget_2->selectionModel();
    QModelIndexList selected = selectionModel->selectedRows();
    if(selected.count() == 0)
        return;

    QModelIndex index = selected.at(0);
    int r = index.row();
    std::string sAddress = ui->tableWidget_2->item(r, 1)->text().toStdString();

    // get existing config entry

}

void MasternodeManager::on_getConfigButton_clicked()
{
    QItemSelectionModel* selectionModel = ui->tableWidget_2->selectionModel();
    QModelIndexList selected = selectionModel->selectedRows();
    if(selected.count() == 0)
        return;

    QModelIndex index = selected.at(0);
    int r = index.row();
    std::string sAddress = ui->tableWidget_2->item(r, 1)->text().toStdString();
    CmastertoadConfig c = pwalletMain->mapMymastertoads[sAddress];
    std::string sPrivKey = c.sMasternodePrivKey;
    mastertoadConfigDialog* d = new mastertoadConfigDialog(this, QString::fromStdString(sAddress), QString::fromStdString(sPrivKey));
    d->exec();
}

void MasternodeManager::on_removeButton_clicked()
{
    QItemSelectionModel* selectionModel = ui->tableWidget_2->selectionModel();
    QModelIndexList selected = selectionModel->selectedRows();
    if(selected.count() == 0)
        return;

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Delete Mastertoad?", "Are you sure you want to delete this mastertoad configuration?", QMessageBox::Yes|QMessageBox::No);

    if(confirm == QMessageBox::Yes)
    {
        QModelIndex index = selected.at(0);
        int r = index.row();
        std::string sAddress = ui->tableWidget_2->item(r, 1)->text().toStdString();
        CmastertoadConfig c = pwalletMain->mapMymastertoads[sAddress];
        CWalletDB walletdb(pwalletMain->strWalletFile);
        pwalletMain->mapMymastertoads.erase(sAddress);
        walletdb.ErasemastertoadConfig(c.sAddress);
        ui->tableWidget_2->clearContents();
        ui->tableWidget_2->setRowCount(0);
        BOOST_FOREACH(PAIRTYPE(std::string, CmastertoadConfig) mastertoad, pwalletMain->mapMymastertoads)
        {
            updatemastertoad(QString::fromStdString(mastertoad.second.sAlias), QString::fromStdString(mastertoad.second.sAddress), QString::fromStdString(mastertoad.second.sMasternodePrivKey), QString::fromStdString(mastertoad.second.sCollateralAddress), QString::fromStdString(""));
        }
    }
}

void MasternodeManager::on_startButton_clicked()
{
    // start the node
    QItemSelectionModel* selectionModel = ui->tableWidget_2->selectionModel();
    QModelIndexList selected = selectionModel->selectedRows();
    if(selected.count() == 0)
        return;

    QModelIndex index = selected.at(0);
    int r = index.row();
    std::string sAddress = ui->tableWidget_2->item(r, 1)->text().toStdString();
    CmastertoadConfig c = pwalletMain->mapMymastertoads[sAddress];

    std::string errorMessage;
    bool result = activeMasternode.RegisterByPubKey(c.sAddress, c.sMasternodePrivKey, c.sCollateralAddress, errorMessage);

    QMessageBox msg;
    if(result)
        msg.setText("Mastertoad at " + QString::fromStdString(c.sAddress) + " started.");
    else
        msg.setText("Error: " + QString::fromStdString(errorMessage));

    msg.exec();
}

void MasternodeManager::on_stopButton_clicked()
{
    // start the node
    QItemSelectionModel* selectionModel = ui->tableWidget_2->selectionModel();
    QModelIndexList selected = selectionModel->selectedRows();
    if(selected.count() == 0)
        return;

    QModelIndex index = selected.at(0);
    int r = index.row();
    std::string sAddress = ui->tableWidget_2->item(r, 1)->text().toStdString();
    CmastertoadConfig c = pwalletMain->mapMymastertoads[sAddress];

    std::string errorMessage;
    bool result = activeMasternode.StopMasterNode(c.sAddress, c.sMasternodePrivKey, errorMessage);
    QMessageBox msg;
    if(result)
    {
        msg.setText("Mastertoad at " + QString::fromStdString(c.sAddress) + " stopped.");
    }
    else
    {
        msg.setText("Error: " + QString::fromStdString(errorMessage));
    }
    msg.exec();
}

void MasternodeManager::on_startAllButton_clicked()
{
    std::string results;
    BOOST_FOREACH(PAIRTYPE(std::string, CmastertoadConfig) mastertoad, pwalletMain->mapMymastertoads)
    {
        CmastertoadConfig c = mastertoad.second;
	std::string errorMessage;
        bool result = activeMasternode.RegisterByPubKey(c.sAddress, c.sMasternodePrivKey, c.sCollateralAddress, errorMessage);
	if(result)
	{
   	    results += c.sAddress + ": STARTED\n";
	}	
	else
	{
	    results += c.sAddress + ": ERROR: " + errorMessage + "\n";
	}
    }

    QMessageBox msg;
    msg.setText(QString::fromStdString(results));
    msg.exec();
}

void MasternodeManager::on_stopAllButton_clicked()
{
    std::string results;
    BOOST_FOREACH(PAIRTYPE(std::string, CmastertoadConfig) mastertoad, pwalletMain->mapMymastertoads)
    {
        CmastertoadConfig c = mastertoad.second;
	std::string errorMessage;
        bool result = activeMasternode.StopMasterNode(c.sAddress, c.sMasternodePrivKey, errorMessage);
	if(result)
	{
   	    results += c.sAddress + ": STOPPED\n";
	}	
	else
	{
	    results += c.sAddress + ": ERROR: " + errorMessage + "\n";
	}
    }

    QMessageBox msg;
    msg.setText(QString::fromStdString(results));
    msg.exec();
}

void MasternodeManager::on_localButton_clicked()
{
    bool bAlreadyHaveLocalNode = false;
    // Check if a local Mastertoad already exists
    BOOST_FOREACH(PAIRTYPE(std::string, CmastertoadConfig) mastertoad, pwalletMain->mapMymastertoads)
    {
        if(mastertoad.second.isLocal)
    {
        bAlreadyHaveLocalNode = true;
        break;
    }
    }
    if(bAlreadyHaveLocalNode)
    {
    QMessageBox msg;
        msg.setText("A local Mastertoad already exists.");
    msg.exec();
    return;
    }

    // Only create once the external IP is known
    if(GetLocalAddress(NULL).ToStringIP() == "0.0.0.0")
    {
    QMessageBox msg;
        msg.setText("The local external IP is not yet detected.  Please try again in a few minutes.");
    msg.exec();
    return;
    }

    if(pwalletMain->GetBalance() < 15000.1*COIN)
    {
    QMessageBox msg;
        msg.setText("You must have at least 15000.1 PEPE to cover the 15000 PEPE collateral for a Mastertoad and the tx fee.");
    msg.exec();
    return;
    }

    if (pwalletMain->IsLocked())
    {
    QMessageBox msg;
        msg.setText("Your wallet must be unlocked so that the 15000 PEPE collateral can be sent.");
    msg.exec();
    return;
    }

    // Automatically create an entry for the local address
    CmastertoadConfig c;
        c.sAlias = "Local Mastertoad";
    c.sAddress = GetLocalAddress(NULL).ToStringIPPort();
        CKey secret;
        secret.MakeNewKey(false);
        c.sMasternodePrivKey = CBitcoinSecret(secret).ToString();
    
        CWalletDB walletdb(pwalletMain->strWalletFile);
        CAccount account;
        walletdb.ReadAccount(c.sAlias, account);
        bool bKeyUsed = false;
    bool bForceNew = false;

        // Check if the current key has been used
        if (account.vchPubKey.IsValid())
        {
            CScript scriptPubKey;
            scriptPubKey.SetDestination(account.vchPubKey.GetID());
            for (map<uint256, CWalletTx>::iterator it = pwalletMain->mapWallet.begin();
                 it != pwalletMain->mapWallet.end() && account.vchPubKey.IsValid();
                 ++it)
            {
                const CWalletTx& wtx = (*it).second;
                BOOST_FOREACH(const CTxOut& txout, wtx.vout)
                    if (txout.scriptPubKey == scriptPubKey)
                        bKeyUsed = true;
            }
        }

        // Generate a new key
        if (!account.vchPubKey.IsValid() || bForceNew || bKeyUsed)
        {
            if (!pwalletMain->GetKeyFromPool(account.vchPubKey))
            {
        QMessageBox msg;
                msg.setText("Keypool ran out, please call keypoolrefill first.");
        msg.exec();
        return;
        }
            pwalletMain->SetAddressBookName(account.vchPubKey.GetID(), c.sAlias);
            walletdb.WriteAccount(c.sAlias, account);
        }

        c.sCollateralAddress = CBitcoinAddress(account.vchPubKey.GetID()).ToString();

        c.isLocal = true;

        pwalletMain->mapMymastertoads.insert(make_pair(c.sAddress, c));
    walletdb.WritemastertoadConfig(c.sAddress, c);
        uiInterface.NotifymastertoadChanged(c);

        strMasterNodeAddr = c.sAddress;
    strMasterNodePrivKey = c.sMasternodePrivKey;

        CKey keyds;
            CPubKey pubkeyds;
    std::string errorMessage;
            if(!darkSendSigner.SetKey(strMasterNodePrivKey, errorMessage, keyds, pubkeyds))
            {
                QMessageBox msg;
                msg.setText("Invalid masternodeprivkey. Please see documenation.");
        msg.exec();
        return;
            }

        activeMasternode.pubKeyMasternode = pubkeyds;
        fMasterNode = true;

    CWalletTx wtx;
    std::string sNarr;

    string strError = pwalletMain->SendMoneyToDestination(CBitcoinAddress(account.vchPubKey.GetID()).Get(), 15000*COIN, sNarr, wtx);
    if (strError != "")
    {
    QMessageBox msg;
        msg.setText(QString::fromStdString(strError));
    msg.exec();
    return;
    }
    else
    {
    QMessageBox msg;
    std::string sMsg = "Local Mastertoad created and 15000 PEPE sent to the collateral address.  Transaction hash:\n";
    sMsg += wtx.GetHash().GetHex();
        msg.setText(QString::fromStdString(sMsg));
    msg.exec();
    return;
    }
}
#include "proofofmeme.h"
#include "ui_proofofmeme.h"

#include "bitcoinunits.h"
#include "guiutil.h"
#include "guiconstants.h"

#include "hash.h"
#include "base58.h"
#include "pubkey.h"
#include "util.h"
#include "init.h"
#include "wallet.h"
#include "walletdb.h"

#include <QScrollArea>
#include <QScroller>

#include <QFileDialog>
#include <QDesktopServices>

#include <fstream>

ProofOfMeme::ProofOfMeme(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProofOfMeme)
{
    ui->setupUi(this);
    fileName = "";
    
}

ProofOfMeme::~ProofOfMeme()
{
    delete ui;
}

void ProofOfMeme::on_filePushButton_clicked()
{
fileName = QFileDialog::getOpenFileName(this,
    tr("Open Meme"), "./", tr("All Files (*.*)"));

    ui->labelFile->setText(fileName);

}

void ProofOfMeme::on_createPushButton_clicked()
{
    if(fileName == "")
    {
  noMemeSelected();   
  return;
    }

    //read whole file
    std::ifstream memeFile;
    memeFile.open(fileName.toStdString().c_str(), std::ios::binary);
std::vector<char> memeContents((std::istreambuf_iterator<char>(memeFile)),
                               std::istreambuf_iterator<char>());

    //hash file
    uint256 memehash = SerializeHash(memeContents);
    CKeyID keyid(Hash160(memehash.begin(), memehash.end()));
    CBitcoinAddress baddr = CBitcoinAddress(keyid);
    std::string addr = baddr.ToString();

    ui->lineEdit->setText(QString::fromStdString(addr));

    CAmount nAmount = 1 * COIN; // 1 meme
    
    // Wallet comments
    CWalletTx wtx;
    wtx.mapValue["comment"] = fileName.toStdString();
    wtx.mapValue["to"]      = "Proof of Meme";
    bool fSubtractFeeFromAmount = true;
    
    if (pwalletMain->IsLocked())
    {
      ui->txLineEdit->setText("ERROR: Wallet is locked! Cannot send proof of meme.");
    }
    else if(pwalletMain->GetBalance() < 1)
    {
      ui->txLineEdit->setText("ERROR: Need 1 PEPE balance to send proof of meme.");
    }
    else
    {
      std::string sNarr;
      std::string strError = pwalletMain->SendMoneyToDestination(baddr.Get(), nAmount, sNarr, wtx);  // transaction issues fix

     if(strError != "")
     {
        QMessageBox infobox;
        infobox.setText(QString::fromStdString(strError));
        infobox.exec();
     }

        ui->txLineEdit->setText(QString::fromStdString(wtx.GetHash().GetHex()));
     }
    

}

void ProofOfMeme::on_checkButton_clicked()
{
    if(fileName == "")
    {
  noMemeSelected();   
  return;
    }

//read whole file
    std::ifstream memeFile;
    memeFile.open(fileName.toStdString().c_str(), std::ios::binary);
std::vector<char> memeContents((std::istreambuf_iterator<char>(memeFile)),
                               std::istreambuf_iterator<char>());

    //hash file
    uint256 memehash = SerializeHash(memeContents);
    CKeyID keyid(Hash160(memehash.begin(), memehash.end()));
    std::string addr = CBitcoinAddress(keyid).ToString();

    //go to block explorer
    std::string bexp = "http://chain.blockpioneers.info/meme/api/v1.php?method=isaddress&address=";
    //open url
    QString link = QString::fromStdString(bexp + addr);
    QDesktopServices::openUrl(QUrl(link));
}

void ProofOfMeme::on_checkTxButton_clicked()
{
  //go to block explorer
    std::string bexp = "http://chain.blockpioneers.info/meme/api/v1.php?method=gettransaction&txid=";
    //open url
    QString link = QString::fromStdString(bexp + ui->txLineEdit->text().toStdString());
    QDesktopServices::openUrl(QUrl(link));
}

void ProofOfMeme::noMemeSelected()
{
  //err message
  QMessageBox errorbox;
  errorbox.setText("No meme selected");
  errorbox.exec();
}

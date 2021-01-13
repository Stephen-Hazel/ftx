// ftx.h - find text buried in some dir's dang files

#ifndef FTX_H
#define FTX_H

#include <QListWidgetItem>
#include "../stv/ui.h"
#include "ui_ftx.h"


QT_BEGIN_NAMESPACE
namespace Ui {class FTx;}
QT_END_NAMESPACE

class FTx : public QMainWindow {
   Q_OBJECT
private:
   Ui::FTx *ui;

public:
   FTx (QWidget *par = nullptr)
   : QMainWindow (par), ui (new Ui::FTx)  {ui->setupUi (this);}

  ~FTx ()       {delete ui;}

   void Open (), Shut ();

   TStr    _s, _dir;                   // what ta search fer, top dir ta search
   StrArr *_tb;                        // filenames havin _s

public slots:
   void Dir ();
   void Find ();
   void Clik ();
   void DClk (int row, int col);
};
#endif // FTX_H

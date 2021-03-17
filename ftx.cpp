// ftx.cpp

#include "ftx.h"

void FTx::DClk (int row, int col)
{ PStr  s, s2, s3;
   (void)col;
//DBG("DClk bgn r=`d", row);
   if (row == -1)  return;

   StrCp (s, UnQS (ui->tblList->item (row, 0)->text ()));
   if (! fork ()) {
      StrCp (s2, (getenv ("VISUAL") == nullptr) ? CC("/opt/app/ned")
                                                : getenv ("VISUAL"));
      if (-1 == system (StrFmt (s3, "`s '`s/`s' `s", s2, _dir, s, _s)))  {};   
      exit (0);
   }
}


void FTx::Clik ()
{ ulong row, r, got = 0, o, m, sln;
  PStr  s1, buf;
  TStr  s, s2;
  char *sp, *psp, *tr, *nt;
  bool  go;
  StrArr tx (CC("txt"), 200000, 100000*sizeof (TStr));
  QColor bl (CC("#000000")), pi (CC("#FF0080"));
   row = ui->tblList->currentRow ();
//DBG("Clik bgn r=`d", row);
   if (row == SC(ulong,-1))  return;

   StrCp (s2, UnQS (ui->tblList->item (row, 0)->text ()));
   StrCp (s, _dir);   StrAp (s, CC("/"));   StrAp (s, s2);
   sln  = StrLn (_s);

//DBG("fn=`s str=`s", s, _s);
   nt = tx.Load (s);
//DBG("nt=`s", nt?nt:"");
//tx.Dump ();

   ui->txtShow->clear ();   ui->txtShow->setTextColor (bl);
   if (nt) {
      ui->txtShow->insertPlainText (
         CC("...doesn't seem to be a regular text file\n\n"));
      return;
   }
   for (r = 0;  r < tx.num;  r++)  if (StrSt (tx.str [r], _s)) {
//DBG("r=`d ngot=`d", r, got);
      if (got >= 100)
         {ui->txtShow->insertPlainText (CC("\n\nENOUGH !!\n"));   break;}

   // div-er if already got any
      if (got) {
         ui->txtShow->setTextColor (pi);
         ui->txtShow->insertPlainText (CC("________\n\n"));
         ui->txtShow->setTextColor (bl);
      }

   // draw pre-find
      m = 4;   if (r < m)  m = r;
      for (o = 0;  o < m;  o++)
         ui->txtShow->insertPlainText (
            StrFmt (s1, "        `s\n", tx.str [r-m+o]));

   // draw 1st found row
      tr = tx.str [r];
      StrFmt (s1, "`>6d  `s\n", r+1, tr);
      tr = & s1 [8];   psp = s1;
      while ((sp = StrSt (tr, _s))) {  // hilite match spots
         MemCp (buf, psp, sp-psp);   buf [sp-psp] = '\0';
         ui->txtShow->insertPlainText (buf);   ui->txtShow->setTextColor (pi);
         MemCp (buf, sp, sln);   buf [sln] = '\0';
         ui->txtShow->insertPlainText (buf);   ui->txtShow->setTextColor (bl);
         tr = psp = sp + sln;
      }
      if (*psp)  ui->txtShow->insertPlainText (psp);

      for (go = true;  go;) {
         got++;
         if (got >= 100)
            {ui->txtShow->insertPlainText (CC("\n\nENOUGH !!\n"));   break;}

      // see if another happens within 4+4 rows, if so do mid n it");
         go = false;
         for (o = 1;  (o <= 9) && (r+o < tx.num);  o++)
            if (StrSt (tx.str [r+o], _s))  {go = true;   break;}
         if (go) {
         // pre-find2
            for (m = 1;  m < o;  m++)
               ui->txtShow->insertPlainText (
                  StrFmt (s1, "        `s\n", tx.str [r+m]));

         // 2nd+ found row
            r += o;
            tr = tx.str [r];
            StrFmt (s1, "`>6d  `s\n", r+1, tr);
            tr = & s1 [8];   psp = s1;
            while ((sp = StrSt (tr, _s))) {      // hilite match spots
               MemCp (buf, psp, sp-psp);   buf [sp-psp] = '\0';
               ui->txtShow->insertPlainText (buf);
               ui->txtShow->setTextColor (pi);
               MemCp (buf, sp, sln);   buf [sln] = '\0';
               ui->txtShow->insertPlainText (buf);
               ui->txtShow->setTextColor (bl);
               tr = psp = sp + sln;
            }
            if (*psp)  ui->txtShow->insertPlainText (psp);
         }
      }

   // draw post-find
      m = 4;   if (r+m >= tx.num)  m = tx.num - r - 1;
      for (o = 0;  o < m;  o++)
         ui->txtShow->insertPlainText (
            StrFmt (s1, "        `s\n", tx.str [r+1+o]));
   }
}


char Buf [8*1024*1024];                // buffer a file to search for str _s

bool DoDir (void *ptr, char dfx, char *fn)
// find any files and put em in _tb
{ TStr end;
//DBG("DoDir dfx=`c fn='`s'", dfx, fn);
   if (dfx == 'd') {
      FnName (end, fn);
      if ((! StrCm (end, CC(".git"))) ||
          (! MemCm (end, CC("build-"), 6)))  return true;   // SKIP !
   }
   if (dfx != 'f')  return false;
  File  f;
  ulong ln = f.Load (fn, Buf, sizeof (Buf));
  FTx *me = (FTx *)ptr;
   if (MemSt (Buf, me->_s, ln))  me->_tb->Add (fn);
   return false;
}


static int TblCmp (void *p1, void *p2)
{  return StrCm (*((char **)p1), *((char **)p2));  }


void FTx::Find ()
// list off files in this dir w matching str;  show em;  search 1st
{ File f;
  StrArr tb (CC("FNLst"), 20000, 13000*sizeof(TStr));
   StrCp (_dir, UnQS (ui->ledDir->text ()));
   StrCp (_s,   UnQS (ui->ledFind->text ()));
DBG("Find dir=`s s=`s", _dir, _s);
   if (*_s == '\0')  {Hey ("I need a string to find...");   return;}

   _tb = & tb;
   f.DoDir (_dir, this, DoDir);
DBG("found `d", tb.num);
   Sort (tb.str, tb.num, sizeof (tb.str [0]), TblCmp);
   ui->tblList->hide ();
   ui->tblList->clearContents ();
   ui->tblList->setRowCount (tb.num);
  QTableWidgetItem *it;
   for (ulong i = 0;  i < tb.num;  i++) {
      it = ui->tblList->item (i, 0);
      if (! it) {
         it = new QTableWidgetItem;
         ui->tblList->setItem (i, 0, it);
      }
      it->setText (& tb.str [i][StrLn (_dir)+1]);
   }
   ui->tblList->resizeColumnsToContents ();
   ui->tblList->show ();

   if (tb.num)  {ui->tblList->selectRow (0);   Clik ();}
}


void FTx::Dir ()
// list off files in this dir w matching str;  show em;  search 1st
{ TStr s;
   StrCp (s, UnQS (ui->ledDir->text ()));
   if (AskDir (this, s, "Pick a top directory"))  ui->ledDir->setText (s);
}


//------------------------------------------------------------------------------
void FTx::Open ()
{ QFont f ("Noto Sans Mono", 12);      // calc our window size from our font
   QApplication::setFont (f);
   WinLoad (this, "StephenHazel", "FTx");
   setWindowTitle ("FindText");

  int argc = qApp->arguments ().count ();
   if (argc >= 2)  StrCp (_s,   CC(UnQS (qApp->arguments ().at (1))));
   else            *_s = '\0';
   if (argc >= 3)  StrCp (_dir, CC(UnQS (qApp->arguments ().at (2))));
   else if (getcwd (_dir, sizeof (TStr)) == nullptr)  *_dir = '\0';

   connect (ui->btnDir,  SIGNAL (clicked ()),       this, SLOT (Dir  ()));
   connect (ui->btnFind, SIGNAL (clicked ()),       this, SLOT (Find ()));
   connect (ui->ledFind, SIGNAL (returnPressed ()), this, SLOT (Find ()));
   connect (ui->tblList, SIGNAL (itemSelectionChanged ()),this, SLOT (Clik ()));
   connect (ui->tblList, SIGNAL (cellDoubleClicked (int,int)),
                                                   this, SLOT (DClk (int,int)));
   ui->tblList->setColumnCount (1);
   ui->tblList->setHorizontalHeaderLabels (QStringList () << "Filename");
   ui->tblList->horizontalHeader ()->setSectionResizeMode(QHeaderView::Stretch);
   ui->tblList->verticalHeader ()->hide ();
   ui->spl->setSizes (QList<int>() << 100 << 300);
   if (*_dir)  ui->ledDir->setText (_dir);
   if (*_s)   {ui->ledFind->setText (_s);   Find ();}
}

void FTx::Shut ()   {WinSave (this, "StephenHazel", "FTx");}

int main (int argc, char *argv [])
{ QApplication a (argc, argv);
  FTx w;
  int r;
   w.Open ();   r = a.exec ();   w.Shut ();   return r;
}

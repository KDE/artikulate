#include <kdialog.h>
#include <klocale.h>

/********************************************************************************
** Form generated from reading UI file 'AppearenceDialogPage.ui'
**
** Created: Thu Jul 25 19:19:58 2013
**      by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_APPEARENCEDIALOGPAGE_H
#define UI_APPEARENCEDIALOGPAGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>
#include <kfontdialog.h>
#include "kfontdialog.h"

QT_BEGIN_NAMESPACE

class Ui_AppearenceDialogPage
{
public:
    QGridLayout *gridLayout;
    KFontChooser *kcfg_Font;

    void setupUi(QWidget *AppearenceDialogPage)
    {
        if (AppearenceDialogPage->objectName().isEmpty())
            AppearenceDialogPage->setObjectName(QString::fromUtf8("AppearenceDialogPage"));
        AppearenceDialogPage->resize(484, 330);
        gridLayout = new QGridLayout(AppearenceDialogPage);
#ifndef UI_Q_OS_MAC
        gridLayout->setSpacing(6);
#endif
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        kcfg_Font = new KFontChooser(AppearenceDialogPage);
        kcfg_Font->setObjectName(QString::fromUtf8("kcfg_Font"));
        QFont font;
        font.setFamily(QString::fromUtf8("Sans Serif"));
        font.setPointSize(48);
        font.setBold(true);
        font.setItalic(false);
        font.setUnderline(false);
        font.setWeight(75);
        font.setStrikeOut(false);
        kcfg_Font->setFont(font);

        gridLayout->addWidget(kcfg_Font, 0, 0, 1, 1);


        retranslateUi(AppearenceDialogPage);

        QMetaObject::connectSlotsByName(AppearenceDialogPage);
    } // setupUi

    void retranslateUi(QWidget *AppearenceDialogPage)
    {
        Q_UNUSED(AppearenceDialogPage);
    } // retranslateUi

};

namespace Ui {
    class AppearenceDialogPage: public Ui_AppearenceDialogPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // AppearenceDialogPage_H


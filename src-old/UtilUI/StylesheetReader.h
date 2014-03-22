/*  ---------------------------------------------------------------------------
 *
 *  @file   style_sheet_reader.h
 *
 *  @author Nick Arapau <infoctopus@gmail.com>
 *
 *  @brief  StyleSheetReader class reads data from files/resources
 * 
 *  @details Usage
 *           In .qrc: <file>ClassName/style.css</file>
 *           In a class method: StyleSheetReader::setStyleSheetFor(this);
 *
 *  ---------------------------------------------------------------------------
 */

#ifndef STYLESHEET_READER_H
#define STYLESHEET_READER_H

#include <QtCore/QFile>

#define NAMESPACE_DELIMITER "::"

namespace CommonUI
{

class StyleSheetReader
{
public:
#ifdef Q_OS_MACX
    static QString readStyleSheet(QString prefix = ":",
                                  QString fileName = "style-mac.css")
#endif

#ifdef Q_OS_WIN
    static QString readStyleSheet(QString prefix = ":",
                                  QString fileName = "style-win.css")
#endif
   {
      QFile file(QString("%1/%2").arg(prefix).arg(fileName));
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         return QString();

      return file.readAll();
   }

   static bool setStyleSheetFor(QWidget *widget)
   {
      QString className = widget->metaObject()->className();
      
      // Removing namespace name, if it's there
      while (className.contains(NAMESPACE_DELIMITER))
      {
         className = className.right(className.size()
            - className.indexOf(NAMESPACE_DELIMITER)
            - QString(NAMESPACE_DELIMITER).size());
      }

      QString css = readStyleSheet(QString(":/%1").arg(className));
      if (!css.isEmpty())
      {
         widget->setStyleSheet(css);
         return true;
      }
      return false;
   }
};

}

#endif
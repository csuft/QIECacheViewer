#include "iecacheviewer.h"
#include <QtWidgets/QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QApplication::setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
	IECacheViewer win;
	win.setStyleSheet(QStringLiteral("*{font-family: 'Î¢ÈíÑÅºÚ'}"));
	win.show();

	return app.exec();
}

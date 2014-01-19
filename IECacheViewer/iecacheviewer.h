#ifndef IECACHEVIEWER_H
#define IECACHEVIEWER_H

#include <QtWidgets/QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QTableView>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QRegExp>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QGroupBox>
#include <QStatusBar>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QIcon>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <string>
#include <vector>

#include "ViewerHelper.h"

using std::string;
using std::wstring;
using std::vector;

class IECacheViewer : public QWidget
{
	Q_OBJECT

public:
	IECacheViewer(QWidget *parent = 0);
	~IECacheViewer();

private slots:
	void onAboutTriggered();
	void onRefreshClicked();
	void onExportClicked();
	void onViewMetaClicked();
	void onLocationChanged(const QModelIndex& index);
	void onFilterExpChanged(const QString& expr);
	void onFilterColumnChanged(int index);
	void onMethodOneChoosed();
	void onMethodTwoChoosed();

private:
	IECacheViewer& operator=(const IECacheViewer& obj);
	IECacheViewer(const IECacheViewer& obj);

private:
	void initTableView(QSortFilterProxyModel* model);
	void initModel();
	void createModelWayOne(QAbstractItemModel* model, int* count);
	void createModelWayTwo(QAbstractItemModel* model, int* count);

private:
	QLineEdit* m_filterExp;
	QComboBox* m_filterColumn;
	QTableView* m_tableView;
	QSortFilterProxyModel* m_proxyModel;
	QStandardItemModel* m_srcModel;

	QStatusBar* m_statusBar;
	QLabel* m_totalCount;					// The total number of items in the tree view.
	QLabel* m_selCount;						// The number of items currently selected.
	QLabel* m_verLabel;

	QRadioButton* m_wayOne;
	QRadioButton* m_wayTwo;
	QGroupBox* m_methodsGroup;
	QPushButton* m_refreshBtn;
	QPushButton* m_exportBtn;
	QPushButton* m_viewIndexMeta;   // the meta information of index.dat file.
	QPushButton* m_closeBtn;

	QMenuBar* m_menuBar;
	QMenu* m_file;
	QMenu* m_help;
	QAction* m_closeAction;
	QAction* m_aboutAction;

	QHBoxLayout* m_topleftLayout;
	QVBoxLayout* m_leftLayout;
	QVBoxLayout* m_methodsLayout;
	QVBoxLayout* m_rightLayout;
	QWidget* m_centralWidget;
	QHBoxLayout* m_mainLayout;
	QVBoxLayout* m_overall;
};

#endif // IECACHEVIEWER_H

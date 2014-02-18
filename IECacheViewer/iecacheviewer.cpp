#include "iecacheviewer.h"

IECacheViewer::IECacheViewer(QWidget *parent)
	: QWidget(parent)
{
	m_menuBar = new QMenuBar;
	m_file = new QMenu(QStringLiteral("&File"), m_menuBar);
	m_help = new QMenu(QStringLiteral("&Help"), m_menuBar);
	m_menuBar->addMenu(m_file);
	m_menuBar->addMenu(m_help);
	m_closeAction = new QAction(QIcon(":/images/4.png"), QStringLiteral("&Close"), m_file);
	m_aboutAction = new QAction(QIcon(":/images/60.png"), QStringLiteral("&About"), m_help);
	m_file->addAction(m_closeAction);
	m_help->addAction(m_aboutAction);
	// Why should we initialize the main layout first? 
	m_mainLayout = new QHBoxLayout;
	m_centralWidget = new QWidget(this);
	m_overall = new QVBoxLayout(this);

	m_filterExp = new QLineEdit(this);
	m_filterExp->setPlaceholderText(QStringLiteral("Filter expression"));
	m_filterColumn = new QComboBox(this);
	m_filterColumn->addItem(QStringLiteral("By File Name"), 0);
	m_filterColumn->addItem(QStringLiteral("By URL Name"), 1);
	m_filterColumn->addItem(QStringLiteral("By Access Time"), 7);
	m_filterColumn->addItem(QStringLiteral("By Modified Time"), 8);

	m_topleftLayout = new QHBoxLayout(this);
	m_topleftLayout->addWidget(m_filterExp, 1);
	m_topleftLayout->addWidget(m_filterColumn);

	// initialize the View/Model architecture.
	m_proxyModel = new QSortFilterProxyModel;
	initModel();
	int count = 0;
	createModelWayOne(m_srcModel, &count);
	m_proxyModel->setSourceModel(m_srcModel);
	m_tableView = new QTableView;
	m_tableView->setSortingEnabled(true);
	m_tableView->sortByColumn(0, Qt::AscendingOrder);
	m_tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	m_tableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	// hide the vertical header since we don't need it at all.
	m_tableView->verticalHeader()->hide();
	m_tableView->setWordWrap(false);
	// resize the width of columns to contents
	m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	// set selection behaviour, watch out the meaning of different argument
	m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	initTableView(m_proxyModel);

	m_leftLayout = new QVBoxLayout(this);
	m_leftLayout->addLayout(m_topleftLayout);
	m_leftLayout->addWidget(m_tableView, 1);

	m_methodsGroup = new QGroupBox(QStringLiteral("Algorithms"), this);
	m_wayOne = new QRadioButton(QStringLiteral("Cache API"), m_methodsGroup);
	m_wayTwo = new QRadioButton(QStringLiteral("Parse index.dat"), m_methodsGroup);
	m_wayOne->setChecked(true);
	m_methodsLayout = new QVBoxLayout(this);
	m_methodsLayout->addWidget(m_wayOne);
	m_methodsLayout->addWidget(m_wayTwo);
	m_methodsGroup->setLayout(m_methodsLayout);

	m_rightLayout = new QVBoxLayout(this);
	m_refreshBtn = new QPushButton(QIcon(":/images/63.png"), QStringLiteral("Refresh"), this);
	m_exportBtn = new QPushButton(QIcon(":/images/55.png"), QStringLiteral("Export"), this);
	m_viewIndexMeta = new QPushButton(QStringLiteral("Meta Info"));
	m_viewIndexMeta->setDisabled(true);
	m_closeBtn = new QPushButton(QStringLiteral("Close"), this);
	
	m_rightLayout->addWidget(m_methodsGroup);
	m_rightLayout->addWidget(m_refreshBtn);
	m_rightLayout->addWidget(m_exportBtn);
	m_rightLayout->addWidget(m_viewIndexMeta);
	m_rightLayout->addStretch(1);;
	m_rightLayout->addWidget(m_closeBtn);
	m_mainLayout->addLayout(m_leftLayout);
	m_mainLayout->addLayout(m_rightLayout);
	m_mainLayout->setContentsMargins(5, 5, 5, 0);
	m_centralWidget->setLayout(m_mainLayout);

	m_statusBar = new QStatusBar;
	m_totalCount = new QLabel(QStringLiteral("%1 items in total").arg(count));
	m_selCount = new QLabel(QStringLiteral("Currently 0 items selected"));
	m_verLabel = new QLabel(QStringLiteral("Version: 0.1"));
	m_statusBar->addWidget(m_totalCount);
	m_statusBar->addWidget(m_selCount);
	m_statusBar->addPermanentWidget(m_verLabel);
	m_statusBar->setSizeGripEnabled(false);
	m_statusBar->setFixedHeight(25);

	m_overall->setContentsMargins(2, 0, 2, 0);
	// This is convenient method to add menu bar to a QDialog or QWidget.
	m_overall->setMenuBar(m_menuBar);   
	m_overall->addWidget(m_centralWidget, 1);
	m_overall->addWidget(m_statusBar);
	setLayout(m_overall);

	setMaximumSize(850, 550);
	setMinimumSize(850, 550);
	setWindowIcon(QIcon(":/images/16.png"));
	setWindowTitle(QStringLiteral("IE Cache Viewer"));

	connect(m_closeAction, SIGNAL(triggered()), this, SLOT(close()));
	connect(m_aboutAction, SIGNAL(triggered()), this, SLOT(onAboutTriggered()));
	connect(m_closeBtn, SIGNAL(clicked()), this, SLOT(close()));
	connect(m_refreshBtn, SIGNAL(clicked()), this, SLOT(onRefreshClicked()));
	connect(m_exportBtn, SIGNAL(clicked()), this, SLOT(onExportClicked()));
	connect(m_viewIndexMeta, SIGNAL(clicked()), this, SLOT(onViewMetaClicked()));
	connect(m_filterExp, SIGNAL(textChanged(const QString&)), this, SLOT(onFilterExpChanged(const QString&)));
	connect(m_filterColumn, SIGNAL(currentIndexChanged(int)), this, SLOT(onFilterColumnChanged(int)));
	connect(m_wayOne, SIGNAL(clicked()), this, SLOT(onMethodOneChoosed()));
	connect(m_wayTwo, SIGNAL(clicked()), this, SLOT(onMethodTwoChoosed()));
	connect(m_tableView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onLocationChanged(const QModelIndex&)));
}

IECacheViewer::~IECacheViewer()
{

}

void IECacheViewer::onAboutTriggered()
{
	QMessageBox::about(this, QStringLiteral("About"), QStringLiteral("The program is licensed under LGPL. \n Author: zhangzhongke"));
}

void IECacheViewer::onRefreshClicked()
{
	m_wayOne->isChecked()?onMethodOneChoosed():onMethodTwoChoosed();
}	

void IECacheViewer::onExportClicked()
{

}

void IECacheViewer::onViewMetaClicked()
{

}

void IECacheViewer::onLocationChanged(const QModelIndex& index)
{
	// update current index in the view
	m_selCount->setText(QStringLiteral("Row:%1 Col:%2").arg(index.row()).arg(index.column()));
}

void IECacheViewer::initTableView(QSortFilterProxyModel* model)
{
	m_tableView->setModel(model);
	m_tableView->setGridStyle(Qt::DotLine);
}

void IECacheViewer::initModel()
{
	m_srcModel = new QStandardItemModel(0, 10, this);
	// initialize and create the header of model.
	m_srcModel->setHeaderData(0, Qt::Horizontal, QStringLiteral("File Name"));
	m_srcModel->setHeaderData(1, Qt::Horizontal, QStringLiteral("Cache URL"));
	m_srcModel->setHeaderData(2, Qt::Horizontal, QStringLiteral("Local Path"));
	m_srcModel->setHeaderData(3, Qt::Horizontal, QStringLiteral("Sub Folder"));
	m_srcModel->setHeaderData(4, Qt::Horizontal, QStringLiteral("Header Info"));
	m_srcModel->setHeaderData(5, Qt::Horizontal, QStringLiteral("Hits"));
	m_srcModel->setHeaderData(6, Qt::Horizontal, QStringLiteral("Size"));
	m_srcModel->setHeaderData(7, Qt::Horizontal, QStringLiteral("Last Access"));
	m_srcModel->setHeaderData(8, Qt::Horizontal, QStringLiteral("Last Modified"));
	m_srcModel->setHeaderData(9, Qt::Horizontal, QStringLiteral("Expiration"));
}

void IECacheViewer::onFilterExpChanged(const QString& expr)
{
	// update the filter expression using CaseInsensitive and FixedString
	QRegExp regExp(m_filterExp->text(), Qt::CaseInsensitive, QRegExp::FixedString);
	m_proxyModel->setFilterRegExp(regExp);
}

void IECacheViewer::onFilterColumnChanged(int index)
{
	// update the filter column upon the change of combo box
	m_proxyModel->setFilterKeyColumn(m_filterColumn->itemData(index).toInt());
}

void IECacheViewer::onMethodOneChoosed()
{
	int count = 0;
	createModelWayOne(m_srcModel, &count);
	// update the total entries in the model
	m_totalCount->setText(QStringLiteral("%1 items total").arg(count));
	m_viewIndexMeta->setDisabled(true);
}

void IECacheViewer::onMethodTwoChoosed()
{
	int count = 0;
	createModelWayTwo(m_srcModel, &count);
	// update the total entries in the model
	m_totalCount->setText(QStringLiteral("%1 items total").arg(count));
	m_viewIndexMeta->setDisabled(false);
}

void IECacheViewer::createModelWayOne(QAbstractItemModel* srcModel, int* count)
{
	srcModel->removeRows(0, srcModel->rowCount());
	// retrive all cache entries using custom built solultions and
	// insert into the table view
	const int ROW = 0;
	ViewerHelper helper;
	vector<CacheEntry> data = helper.getCacheEntVec();
	for (vector<CacheEntry>::const_iterator ci = data.cbegin(); ci != data.end(); ++ci)
	{
		*count += 1;
		srcModel->insertRow(ROW);
		srcModel->setData(srcModel->index(ROW, 0), (*ci).m_fileName.c_str());
		srcModel->setData(srcModel->index(ROW, 1), (*ci).m_urlStr.c_str());
		srcModel->setData(srcModel->index(ROW, 2), (*ci).m_localPath.c_str());
		srcModel->setData(srcModel->index(ROW, 3), (*ci).m_subFolder.c_str());
		srcModel->setData(srcModel->index(ROW, 4), (*ci).m_headerInfo.c_str());
		srcModel->setData(srcModel->index(ROW, 5), (*ci).m_hits);
		srcModel->setData(srcModel->index(ROW, 6), (*ci).m_entrySize);
		srcModel->setData(srcModel->index(ROW, 7), (*ci).m_lastAccess.c_str());
		srcModel->setData(srcModel->index(ROW, 8), (*ci).m_lastModified.c_str());
		srcModel->setData(srcModel->index(ROW, 9), (*ci).m_expiration.c_str());
	}
}

void IECacheViewer::createModelWayTwo(QAbstractItemModel* srcModel, int* count)
{
	srcModel->removeRows(0, srcModel->rowCount());
	// retrive all cache entries using custom built solultions and
	// insert into the table view
	const int ROW = 0;
	IndexParser parser;
	vector<CacheEntry> data = parser.getCacheEntVec();
	for (vector<CacheEntry>::const_iterator ci = data.cbegin(); ci != data.end(); ++ci)
	{
		*count += 1;
		srcModel->insertRow(0);
		srcModel->setData(srcModel->index(ROW, 0), (*ci).m_fileName.c_str());
		srcModel->setData(srcModel->index(ROW, 1), (*ci).m_urlStr.c_str());
		srcModel->setData(srcModel->index(ROW, 2), (*ci).m_localPath.c_str());
		srcModel->setData(srcModel->index(ROW, 3), (*ci).m_subFolder.c_str());
		srcModel->setData(srcModel->index(ROW, 4), (*ci).m_headerInfo.c_str());
		srcModel->setData(srcModel->index(ROW, 5), (*ci).m_hits);
		srcModel->setData(srcModel->index(ROW, 6), (*ci).m_entrySize);
		srcModel->setData(srcModel->index(ROW, 7), (*ci).m_lastAccess.c_str());
		srcModel->setData(srcModel->index(ROW, 8), (*ci).m_lastModified.c_str());
		srcModel->setData(srcModel->index(ROW, 9), (*ci).m_expiration.c_str());
	}
}
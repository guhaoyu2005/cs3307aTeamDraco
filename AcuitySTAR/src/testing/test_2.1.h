#ifndef TEST_1_2_H
#define TEST_1_2_H

#include <string>
#include <vector>
#include <QObject>
#include <QDebug>
#include <QStringList>
#include <QtTest/QtTest>

#include "./database/CSVReader.h"
#include "database/RecordsManager.h"
#include "datamodel/TreeItem.h"
#include "datamodel/TreeModel.h"
#include "datamodel/TeachingTreeModel.h"
#include "datamodel/PublicationTreeModel.h"
#include "datamodel/PresentationTreeModel.h"
#include "datamodel/GrantFundingTreeModel.h"

/**
 * @brief TC_2.1 - The QTTestTreeViewClick class
 * @class QTTestTreeViewClick
 * @author James Wake
 * @date 2016/10/19
 * @details
 * TC_2.1 - QT Test Class for Tree Models
 */
class QTTestTreeModel: public QObject
{   
    Q_OBJECT
private:
    TeachingTreeModel *teaching;
    QStringList *teachingOrder;
    PublicationTreeModel *publication;
    QStringList *publicationOrder;
    PresentationTreeModel *presentation;
    QStringList *presentationOrder;
    GrantFundingTreeModel *grant;
    QStringList *grantOrder;

private slots:

    /**
     * @brief initTestCase
     * @class QTTestTreeModel
     * @author James Wake
     * @date 2016/10/20
     * @details
     * Initialize the Test Case
     */
    void initTestCase();

    void testTeachingTree();

    /**
     * @brief cleanupTestCase
     * @class QTTestTreeModel
     * @author James Wake
     * @date 2016/10/20
     * @details
     * Cleanup Tests
     */
    void cleanupTestCase();
};

#endif // TEST_1_2_H

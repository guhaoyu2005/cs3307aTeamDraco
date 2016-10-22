#include "./test_2.1.h"
using namespace std;

void QTTestTreeModel::initTestCase()
{
   //Setup Teaching Tree Model for Testing
   CSVReader reader("../Project Information/Sample Data/Program_Teaching_expanded.csv");
   vector<string> teachingHeader = reader.getHeaders();
   RecordsManager teachingRecords = new RecordsManager(&teachingHeader);
//   this->teaching = new TeachingTreeModel();
//   QString teachingOrder = new QString();
//   teachingOrder << "Member Name" << "Program";
//   this->teaching.setupModel(2010, 2015, teachingOrder, "Member Name", "Program");

//    //Setup Publication Tree Model for Testing
//    this->publication = new PublicationTreeModel();
//    this->publicationOrder << "Member Name" << "Type" << "Role" << "Title";
//    this->publication.setupModel(2010, 2015, this->publicationOrder, "Member Name", "Title");

//    //Setup Presentation Tree Model for Testing
//    this->presentation = new PresentationTreeModel();
//    this->presentationOrder << "Member Name" << "Type";
//    this->presentation.setupModel(2010, 2015, this->presentationOrder, "Member Name", "Type");

//    //Setup Presentation Tree Model for Testing
//    this->grant = new GrantFundingTreeModel();
//    this->grantOrder << "Member Name" << "Funding Type" << "Peer Reviewed?" << "Status" << "Role" << "Title";
//    this->grant.setupModel(2010, 2015, this->grantOrder, "Member Name", "Title");

   qDebug("TC_2.1 Test Tree Model Starting");
}

void QTTestTreeModel::testTeachingTree()
{
    qDebug("TC_2.1 Testing Teaching Tree Model");
}

void QTTestTreeModel::cleanupTestCase()
{
   qDebug("TC_2.1 Test Tree Model Finished");
}

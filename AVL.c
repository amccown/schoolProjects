/* Self Balancing Binary Search Tree.  c 2016 Alexander McCown
Permission granted to use and modify my code for non commercial use provided this 
commment is included and citation is made */



#include "AVL.h"
#include "Node.h"

void AVL::rotateLeft(Node*& localRoot)
{
 //cout << "\nRotate left ";
 Node* toSwapUp = localRoot->right;
 localRoot->right = toSwapUp->left;
 toSwapUp->left = localRoot;
 localRoot = toSwapUp;
 toSwapUp = NULL;
}

void AVL::rotateRight(Node*& localRoot)
{
 //cout << "\nRotate Right";
 Node* toSwapUp = localRoot->left;
 localRoot->left = toSwapUp->right;
 toSwapUp->right = localRoot;
 localRoot = toSwapUp;
 toSwapUp = NULL;
}

void AVL::balanceTo(Node*& localRoot, int POTD)
{
 if(localRoot->data < POTD)
 {
  if(localRoot->right != NULL)
  balanceTo(localRoot->right, POTD);
 }
 else if(localRoot->data > POTD)
 {
  if(localRoot->left != NULL)
  balanceTo(localRoot->left, POTD);
 }
 else
 {

 }
 if((localRoot->getBalance() > 1)||(localRoot->getBalance() < -1))
 balanceIt(localRoot);
}


void AVL::balanceIt(Node*& localRoot)
{
// cout << "\nbalancing " << localRoot->data << "\n";
// cout << " who's children are : " ;
// if(localRoot->left != NULL) cout << "left " << localRoot->left->data;
// if(localRoot->right != NULL) cout <<" and right " << localRoot->right->data;

 if(localRoot->getBalance() == 2)  // Right heavy tree.
 {
  if(localRoot->right->getBalance() < 0)      // Right-left tree.  Right rotation of right child.
  {
   rotateRight(localRoot->right);
  }
              // Right-right tree.   do a left rotation on the localRoot.
  rotateLeft(localRoot);
 }
 else   //left heavy tree
 {
  if(localRoot->left->getBalance() > 0)     // left-right tree.  Left rotation of the left child
  {
   rotateLeft(localRoot->left);
  }
              // left-left tree.  Do a right rotation on the localRoot.
  rotateRight(localRoot);
 }
}


 //This is a test function used to count the number of nodes in a tree

 int AVL::countNodes(Node* localRoot)
 {
        // Count the nodes in the binary tree to which
        // localRoot points, and return the answer.
  if ( localRoot == NULL )
   return 0;  // The tree is empty.  It contains no nodes.
  else
  {
   int count = 1;   // Start by counting the root.
   count += countNodes(root->left);  // Add the number of nodes
            //     in the left subtree.
   count += countNodes(root->right); // Add the number of nodes
            //    in the right subtree.
   return count;  // Return the total.
  }
 }

 bool AVL::isInTree(int dataToFind, Node*& localRoot)
 {
  bool isIn = false;
  if(localRoot == NULL)
  {

  }
  else
  {
   if ( localRoot->data == dataToFind )
    isIn= true;
   else
   {
    if(isInTree(dataToFind, localRoot->left)||(isInTree(dataToFind, localRoot->right)))
    isIn = true;
   }
  }
   return isIn;
 }

 void AVL::insertInt(int data, Node*& localRoot)
 {
  if(localRoot == NULL)
  {
   localRoot = new Node(data);
   localRoot->left = NULL;
   localRoot->right = NULL;
  }
  else if(localRoot->data > data)
  {
 //  cout << "\nI think  " << data << " is less than it's root " << localRoot->data;
   insertInt(data, localRoot->left);
  }
  else if(localRoot->data < data)
  {
   insertInt(data, localRoot->right);
  }
  else
  {
    cout << "\nSomething went wrong, encountered duplicate: " << data << endl;
  }

 // cout << " balance at node: " << localRoot->data  << " is "<< localRoot->getBalance();
  if((localRoot->getBalance() > 1)||(localRoot->getBalance() < -1))
  {
   balanceIt(localRoot);
  }
 }


 /*
 * Returns the root node for this tree
 *
 * @return the root node for this tree.
 */
 NodeInterface * AVL::getRootNode()
 {
  return root;
 }

 /*
 * Attempts to add the given int to the AVL tree
 *
 * @return true if added
 * @return false if unsuccessful (i.e. the int is already in tree)
 */
 bool AVL::add(int data)
 {
  bool isSuccess = false;
  if(root == NULL)
  {
   insertInt(data,root);
   isSuccess = true;
  }
  else
  {
   if(isInTree(data,root) == false)
   {
    // cout << "\ntrying to insert";
    insertInt(data,root);
    isSuccess = true;
   }
   else
   {
    // cout << "\nAlready in tree";
    isSuccess = false;
   }
  }
  return isSuccess;
 }
 void AVL::removeInt(int data, Node* localRoot)
 {
 // cout << "\nTrying to remove: " << data;
 // cout << " who's children are : " ;
 // if(localRoot->left != NULL) cout << "left " << localRoot->left->data;
 // if(localRoot->right != NULL) cout <<" and right " << localRoot->right->data;

  // Find the item
  Node* predecessor = NULL;
  Node* current = localRoot;

  if(current==NULL)
  {
   // cout<<"Tree is empty"<<endl;
   return;
  }
  while((current!=NULL)&&(current->data!=data))
  {
   if(current->data==data)
   {
   }
   else
   {
    predecessor = current;
    if(data > (current->data))
     current=current->right;
    else
     current=current->left;
   }
  }
  if(current->data!=data)
  {
   // cout<< endl << data<<" not in Tree."<<endl;
   return;
  }

  // CASE 2: Removing a Leaf Node
   if((current->left == NULL) && (current->right == NULL))    // ie, if it's a leaf
   {
    // cout << "\nCase 2\n";// << current->getData();
    if(root->data == current->data)
    {
     root = NULL;
     delete current;
    }
    else
    {
     if(predecessor == NULL)
     {
      root = NULL;
      delete current;
     }
     else
     {
      if(predecessor->left==current)
       predecessor->left=NULL;
      else
       predecessor->right=NULL;
     delete current;
     }


     int ParentOfTheDeceased = predecessor->data;
     //cout << endl << ParentOfTheDeceased;///This is used in iterating down to the delete node
     balanceTo(root, ParentOfTheDeceased);
    }
   // cout<<data<<" has been removed from the Tree."<<endl;
   return;
  }

  // CASE 1: Removing a node with a single child
   else if(((current->left==NULL ) && (current->right != NULL)) || ((current->left != NULL) && (current->right==NULL)))
  {
   // cout << "\nCase 1";
   // Right Leaf Present, No Left Leaf
   if(current->left==NULL && current->right != NULL)
   {
    if(predecessor == NULL)
    {
     Node* temp = current;
     root = current->right;
     delete temp;
    }
    else
    {
     // If predecessor's left tree equals Node n
     if(predecessor->left==current)
     {
      // then predecessor's left tree becomes n's right tree
      // and delete n
      predecessor->left=current->right;
      delete current;
      current=NULL;
      // cout<<data<<" has been removed from the Tree."<<endl;
     }
     // If predecessor's right tree equals Node n
     else
     {
      // then predecessor's right tree becomes n's right tree
      // and delete n
      predecessor->right=current->right;
      delete current;
      current=NULL;
      // cout<<data<<" has been removed from the Tree."<<endl;
     }

     int ParentOfTheDeceased = predecessor->data;       ///This is used in iterating down to the delete node
     balanceTo(root, ParentOfTheDeceased);
    }
   }
   else // Left Leaf Present, No Right Leaf Present
   {
    if(predecessor == NULL)       // ie, current is the root.  This shouldn't need any balancing
    {
     Node* temp = current;
     root = current->left;
     delete temp;
    }
    else
    {
     if(predecessor->left==current)
     {
      predecessor->left=current->left;
      delete current;
      current=NULL;
      // cout<<data<<" has been removed from the Tree."<<endl;
     }
     else
     {
      predecessor->right=current->left;
      delete current;
      current=NULL;
      // cout<<data<<" has been removed from the Tree."<<endl;
     }
     int ParentOfTheDeceased = predecessor->data;       ///This is used in iterating down to the delete node
     balanceTo(root, ParentOfTheDeceased);
    }
   }
   return;
  }

  // CASE 3: Node has two children
  // Replace Node with smallest value in right subtree
  else if((current->left != NULL) && (current->right != NULL))
  {
   //cout << "\nCase 3";
  // // cout << endl << "Right leaf not Null";
   Node* check=current->left;
   if((check->left==NULL)&&(check->right==NULL))      //Left child is a leaf
   {
    current->data =check->data;
    delete check;
    current->left = NULL;

    int ParentOfTheDeceased = current->data;       ///This is used in iterating down to the delete node
    balanceTo(root, ParentOfTheDeceased);
    // cout<<data<<" has been removed from the Tree."<<endl;
   }
   else // left child has children
   {
    if(current->left->right == NULL)
    {
     check = current->left;
     current->data = check->data;
     current->left = check->left;
     delete check;

     int ParentOfTheDeceased = current->data;       ///This is used in iterating down to the delete node
     balanceTo(root, ParentOfTheDeceased);
    }
    else
    {
     Node* rightCurrent = current->left->right;
     Node* rightCurrentPred = current->left;   //find the one to delete;
     while(rightCurrent->right != NULL)
     {
      rightCurrentPred = rightCurrent;
      rightCurrent = rightCurrent->right;
     }
     current->data = rightCurrent->data;
     rightCurrentPred->right= rightCurrent->left;
     delete rightCurrent;

     int ParentOfTheDeceased = rightCurrentPred->data;       ///This is used in iterating down to the delete node
     balanceTo(root, ParentOfTheDeceased);
    // rightCurrentPred->right = NULL;
    }
   }
    // cout << endl<< getRootNode()->getData();
   return;
  }
  else
  {
   // cout << "\nError, failed to find appropriate action";
  }
 }


 /*
 * Attempts to remove the given int from the AVL tree
 *
 * @return true if successfully removed
 * @return false if remove is unsuccessful(i.e. the int is not in the tree)
 */
 bool AVL::remove(int data)
 {
  bool isSuccess = false;
  if(isInTree(data, root))
  {
   /*  // cout << "\nTrying to remove " << data;
   // cout << "\ncurrent root is " << root->getData();
  if(root->getLeftChild() == NULL)
   {
   // cout << "\nleft: " << root->getLeftChild();
   // cout << "\nright: " << root->getRightChild();
   }
*/
   removeInt(data, root);
   isSuccess = true;
  }
  else
   isSuccess = false;
  return isSuccess;
 }

 /*
 * Removes all nodes from the tree, resulting in an empty tree.
 */
 void AVL::clear()
 {
  while(root != NULL)
  {
   // cout << "\nTrying to remove: " << root->data;
  // // cout << "\nchildren: " << root->getRightChild()->getData();
   remove(root->data);
  }
 }

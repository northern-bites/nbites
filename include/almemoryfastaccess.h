/**
* @author Aldebaran Robotics 
* Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.\n
*
* Version : $Id$
*/


#ifndef ALMemoryFastAccess_H
# define ALMemoryFastAccess_H


/**
*   Propose a fast access in Read/Write to some variables of the stm regularily accessed, with a unified interface.
*
*   - if stm is in same broker than the caller: store directly pointer to data (hack* method)
*   - if stm is remote with the caller: standard call (not so optimised, but the more optimal possible access)
*
*   - ASSUME: variables never disappear.
*   - ASSUME: variables are all floats.
*
*/

#include "altypes.h"
#include "alxplatform.h"  // pour la normalisation de DEBUG et _DEBUG
#include "alvalue.h"
#include "alptr.h"
#include "albroker.h"

class value;

namespace AL
{
  class ALProxy; // class AlMemoryProxy
}

class ALMemoryFastAccess
{
  typedef std::vector<value*>               TVariablePtrArray;
  typedef TVariablePtrArray::iterator       ITVariablePtrArray;
  typedef TVariablePtrArray::const_iterator CITVariablePtrArray;

public:
  /**
  * Default Constructor.
  */
  ALMemoryFastAccess();

  ~ALMemoryFastAccess();


  static void StopAllAccess( void );


  /**
  * ConnectToVariables. First time, you must "connect" to variables to access later, many times
  * @param pListVariables List of variable names, eg: {"DCM/Sensor1", "DCM/Sensor2", "DCM/Sensor3" } ...
  * @param strCallerName Name of the caller (the local module that asks for a fast access)
  * @param bAllowNonExistantVariable If true this will create keys in Memory for the keys that don't exist.
  */
  void ConnectToVariables( const AL::ALPtr<AL::ALBroker> pBroker, const TStringArray & pListVariables, bool bAllowNonExistantVariable = false );

  //
  // Then access them many times...
  //

  /**
  * GetValues. Access previously "connected" variables.
  * @param pListValue Where to put the gathered value of variables.
  */
  void GetValues( TFloatArray & pListValue ) const;

  /**
  * SetValues. Access previously "connected" variables.
  * @param pListValue Value of variables to put into memory.
  */
  void SetValues( const TFloatArray & pListValue );

  /**
  * DisableCheckOnNotExistingValueOrThings_Debug. Disable fatal error, if values connecting doesn't exists
  * WARNING: this is a debug functionnality only.
  */
#ifdef DEBUG
  inline void DisableCheckOnNotExistingValueOrThings_Debug( void ) { fbDisableCheking_Debug = true; };
#endif

  /**
  * InnerTest. 
  * return true if ok
  */
  static bool InnerTest( const AL::ALPtr<AL::ALBroker> pBroker );

private:

  AL::ALProxy*        fStm;               // connection to stm

  bool                fbIsLocal;          // stm is local to us
  TVariablePtrArray   fListVariablePtr;   // local case: pointer to the direct memory access where the data is stored
  ALValue             fCommandToGetData; // remote case: the format of the GetListData Command to send
  ALValue             fCommandToSetData; // remote case: the format of the InsertListData Command to send

#ifdef DEBUG
  bool                fbDisableCheking_Debug;   // if true, doesn't complaign if value doesn't exists
#endif

  static bool         static_bStopAllAccess;    // if true, stop all access (usually ALMemory has been destroyed)

};
#endif // ALMemoryFastAccess_H

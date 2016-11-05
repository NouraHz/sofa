/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2016 INRIA, USTL, UJF, CNRS, MGH                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                               SOFA :: Modules                               *
*                                                                             *
* This component is open-source                                               *
*                                                                             *
* Contributors:                                                               *
*    - damien.marchal@univ-lille1.fr                                          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include <gtest/gtest.h>

#include <string>
using std::string ;

#include <SofaSimulationGraph/DAGSimulation.h>
using sofa::simulation::graph::DAGSimulation ;

#include <sofa/simulation/Simulation.h>
using sofa::simulation::Simulation ;

#include <sofa/simulation/Node.h>
using sofa::simulation::Node ;

#include <SofaSimulationCommon/SceneLoaderXML.h>
using sofa::simulation::SceneLoaderXML ;

#include <SofaComponentBase/initComponentBase.h>

#include <SofaComponentBase/MakeDataAliasComponent.h>
using sofa::component::MakeDataAliasComponent ;

//TODO(dmarchal): all these lines are ugly...this is too much for simple initialization stuff.
#include <SofaTest/TestMessageHandler.h>
#include <sofa/helper/logging/ConsoleMessageHandler.h>
using sofa::helper::logging::MessageDispatcher;
using sofa::helper::logging::MessageHandler;
using sofa::helper::logging::ConsoleMessageHandler;
using sofa::helper::logging::Message ;

#include <SofaTest/LogMessage.h>
using sofa::helper::logging::MainLogginMessageHandler ;
using sofa::helper::logging::MainCountingMessageHandler ;
using sofa::helper::logging::ExpectMessage ;
using sofa::helper::logging::MessageAsTestFailure ;
using sofa::helper::logging::LogMessage ;

#include <sofa/helper/logging/RichConsoleStyleMessageFormatter.h>
using sofa::helper::logging::RichConsoleStyleMessageFormatter ;

using sofa::core::objectmodel::ComponentState ;

namespace makedataaliascomponent_test
{

MessageHandler* defaultHandler=nullptr ;
Simulation* theSimulation = nullptr ;

bool doInit(){
    sofa::component::initComponentBase();
    return true;
}

bool inited = doInit();

void perTestInit()
{
    if(theSimulation==nullptr){
        theSimulation = new DAGSimulation();
        sofa::simulation::setSimulation(theSimulation);
    }

    if(defaultHandler==nullptr)
        defaultHandler=new ConsoleMessageHandler(new RichConsoleStyleMessageFormatter) ;

    MessageDispatcher::clearHandlers() ;
    MessageDispatcher::addHandler( &MainCountingMessageHandler::getInstance() ) ;
    MessageDispatcher::addHandler( &MainLogginMessageHandler::getInstance() ) ;
}

TEST(MakeDataAliasComponent, checkGracefullHandlingOfMissingAttributes)
{
    perTestInit();
    ExpectMessage e(Message::Error) ;

    string scene =
        "<?xml version='1.0'?>                                               "
        "<Node 	name='Root' gravity='0 0 0' time='0' animate='0'   >         "
        "       <MakeDataAlias/>                                             "
        "</Node>                                                             " ;

    Node::SPtr root = SceneLoaderXML::loadFromMemory ( "test1",
                                                       scene.c_str(),
                                                       scene.size() ) ;
    EXPECT_TRUE(root!=nullptr) ;

    MakeDataAliasComponent* component = nullptr;

    root->getTreeObject(component) ;
    EXPECT_TRUE(component!=nullptr) ;

    EXPECT_EQ(component->getComponentState(), ComponentState::Invalid) ;

    theSimulation->unload(root) ;
}

TEST(MakeDataAliasComponent, checkGracefullHandlingOfMissingTargetAttributes)
{
    perTestInit();
    ExpectMessage e(Message::Error) ;
    LogMessage logger;

    string scene =
        "<?xml version='1.0'?>                                               "
        "<Node 	name='Root' gravity='0 0 0' time='0' animate='0'   >         "
        "       <MakeDataAlias                             alias='NewName'/>      "
        "</Node>                                                             " ;

    Node::SPtr root = SceneLoaderXML::loadFromMemory ( "test1",
                                                       scene.c_str(),
                                                       scene.size() ) ;
    EXPECT_TRUE(root!=nullptr) ;

    MakeDataAliasComponent* component = nullptr;

    root->getTreeObject(component) ;
    EXPECT_TRUE(component!=nullptr) ;
    EXPECT_EQ(component->getComponentState(), ComponentState::Invalid) ;

    theSimulation->unload(root) ;
}

TEST(MakeDataAliasComponent, checkGracefullHandlingOfMissingAliasAttributes)
{
    perTestInit();
    ExpectMessage e(Message::Error) ;

    string scene =
        "<?xml version='1.0'?>                                               "
        "<Node 	name='Root' gravity='0 0 0' time='0' animate='0'   >         "
        "       <MakeDataAlias targetcomponent='MakeAlias'/>                     "
        "</Node>                                                             " ;

    Node::SPtr root = SceneLoaderXML::loadFromMemory ( "test1",
                                                       scene.c_str(),
                                                       scene.size() ) ;
    EXPECT_TRUE(root!=nullptr) ;

    MakeDataAliasComponent* component = nullptr;

    root->getTreeObject(component) ;
    EXPECT_TRUE(component!=nullptr) ;
    EXPECT_EQ(component->getComponentState(), ComponentState::Invalid) ;

    theSimulation->unload(root) ;
}

TEST(MakeDataAliasComponent, checkGracefullHandlingOfInvalidTargetName)
{
    perTestInit();
    ExpectMessage e(Message::Error) ;

    string scene =
        "<?xml version='1.0'?>                                               \n"
        "<Node 	name='Root' gravity='0 0 0' time='0' animate='0'   >         \n"
        "       <MakeDataAlias componentname='InvalidComponentName' dataname='position' alias='rest_position'/> \n"
        "</Node>                                                             \n" ;

    Node::SPtr root = SceneLoaderXML::loadFromMemory ( "test1",
                                                       scene.c_str(),
                                                       scene.size() ) ;
    EXPECT_TRUE(root!=nullptr) ;

    MakeDataAliasComponent* component = nullptr;

    root->getTreeObject(component) ;
    EXPECT_TRUE(component!=nullptr) ;
    EXPECT_EQ(component->getComponentState(), ComponentState::Invalid) ;

    theSimulation->unload(root) ;
}

TEST(MakeDataAliasComponent, checkGracefullHandlingOfInvalidDataName)
{
    perTestInit();
    ExpectMessage e(Message::Error) ;

    string scene =
        "<?xml version='1.0'?>                                               \n"
        "<Node 	name='Root' gravity='0 0 0' time='0' animate='0'   >         \n"
        "       <MakeDataAlias componentname='MechanicalObject' dataname='invalidname' alias='myrest_position'/> \n"
        "</Node>                                                             \n" ;

    Node::SPtr root = SceneLoaderXML::loadFromMemory ( "test1",
                                                       scene.c_str(),
                                                       scene.size() ) ;
    EXPECT_TRUE(root!=nullptr) ;
    MakeDataAliasComponent* component = nullptr;

    root->getTreeObject(component) ;
    EXPECT_TRUE(component!=nullptr) ;
    EXPECT_EQ(component->getComponentState(), ComponentState::Invalid) ;

    theSimulation->unload(root) ;
}

TEST(MakeDataAliasComponent, checkValidBehavior)
{
    MessageAsTestFailure check(Message::Error) ;

    string scene =
        "<?xml version='1.0'?>                                               \n"
        "<Node 	name='Root' gravity='0 0 0' time='0' animate='0'   >         \n"
        "       <MakeDataAlias componentname='MechanicalObject' dataname='position' alias='myrest_position'/> \n"
        "       <MechanicalObject myrest_position='1 2 3 4'/>                                                 \n"
        "</Node>                                                             \n" ;

    Node::SPtr root = SceneLoaderXML::loadFromMemory ( "test1",
                                                       scene.c_str(),
                                                       scene.size() ) ;
    EXPECT_TRUE(root!=nullptr) ;

    MakeDataAliasComponent* component = nullptr;
    root->getTreeObject(component) ;

    EXPECT_TRUE(component!=nullptr) ;
    EXPECT_EQ(component->getComponentState(), ComponentState::Valid) ;

    theSimulation->unload(root) ;
}

}
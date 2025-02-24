/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU General Public License as published by the Free  *
* Software Foundation; either version 2 of the License, or (at your option)   *
* any later version.                                                          *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
* more details.                                                               *
*                                                                             *
* You should have received a copy of the GNU General Public License along     *
* with this program. If not, see <http://www.gnu.org/licenses/>.              *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include <sstream>
using std::ostringstream ;
#include <fstream>

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <boost/program_options.hpp>

#include "SimpleGUI.h"

#include <sofa/helper/ArgumentParser.h>
#include <SofaSimulationCommon/config.h>
#include <sofa/simulation/Node.h>
#include <sofa/helper/system/PluginManager.h>
#include <sofa/simulation/config.h> // #defines SOFA_HAVE_DAG (or not)
#include <SofaSimulationCommon/init.h>
#include <SofaSimulationGraph/init.h>
#include <SofaSimulationGraph/DAGSimulation.h>
using sofa::simulation::Node;

#include <SofaCommon/initSofaCommon.h>
#include <SofaBase/initSofaBase.h>
#include <SofaGeneral/initSofaGeneral.h>

#include <SofaGeneralLoader/ReadState.h>
#include <SofaValidation/CompareState.h>
#include <sofa/helper/Factory.h>
#include <sofa/helper/cast.h>
#include <sofa/helper/BackTrace.h>
#include <sofa/helper/system/FileRepository.h>
#include <sofa/helper/system/SetDirectory.h>
#include <sofa/helper/Utils.h>
#include <sofa/gui/GUIManager.h>
using sofa::gui::GUIManager;

#include <sofa/gui/Main.h>
#include <sofa/gui/BaseGUI.h>
#include <sofa/helper/system/gl.h>

using sofa::core::ExecParams ;

#include <sofa/helper/system/console.h>
using sofa::helper::Utils;

using sofa::component::misc::CompareStateCreator;
using sofa::component::misc::ReadStateActivator;
using sofa::simulation::tree::TreeSimulation;
using sofa::simulation::graph::DAGSimulation;
using sofa::helper::system::SetDirectory;
using sofa::core::objectmodel::BaseNode ;
using sofa::gui::BaseGUI;

#include <sofa/helper/logging/Messaging.h>

#include <sofa/helper/logging/ConsoleMessageHandler.h>
using sofa::helper::logging::ConsoleMessageHandler ;

#include <sofa/core/logging/RichConsoleStyleMessageFormatter.h>
using  sofa::helper::logging::RichConsoleStyleMessageFormatter ;

#include <sofa/core/logging/PerComponentLoggingMessageHandler.h>
using  sofa::helper::logging::MainPerComponentLoggingMessageHandler ;

#ifdef WIN32
#include <windows.h>
#endif

using sofa::helper::system::DataRepository;
using sofa::helper::system::PluginRepository;
using sofa::helper::system::PluginManager;

#include <sofa/helper/logging/MessageDispatcher.h>
using sofa::helper::logging::MessageDispatcher ;

#include <sofa/helper/logging/ClangMessageHandler.h>
using sofa::helper::logging::ClangMessageHandler ;

#include <sofa/helper/logging/ExceptionMessageHandler.h>
using sofa::helper::logging::ExceptionMessageHandler;

// ---------------------------------------------------------------------
// ---
// ---------------------------------------------------------------------
int main(int argc, char** argv)
{
    sofa::helper::BackTrace::autodump();

    ExecParams::defaultInstance()->setAspectID(0);

#ifdef WIN32
    {
        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD s;
        s.X = 160; s.Y = 10000;
        SetConsoleScreenBufferSize(hStdout, s);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(hStdout, &csbi))
        {
            SMALL_RECT winfo;
            winfo = csbi.srWindow;
            //winfo.Top = 0;
            winfo.Left = 0;
            //winfo.Bottom = csbi.dwSize.Y-1;
            winfo.Right = csbi.dwMaximumWindowSize.X-1;
            SetConsoleWindowInfo(hStdout, TRUE, &winfo);
        }

    }
#endif

    string fileName ;
    bool        showHelp = false;
    bool        startAnim = false;
    bool        loadRecent = false;
    bool        printFactory = false;
    bool        noAutoloadPlugins = false;
    bool        temporaryFile = false;

    string simulationType = "dag";

    vector<string> plugins;
    vector<string> files;
    string colorsStatus = "auto";
    string messageHandler = "auto";
    bool enableInteraction = false ;
    string gui = "";

    string gui_help = "choose the UI (";
    gui_help += GUIManager::ListSupportedGUI('|');
    gui_help += ")";

    ArgumentParser* argParser = new ArgumentParser(argc, argv);

    argParser->addArgument(boost::program_options::value<bool>(&showHelp)->default_value(false)->implicit_value(true), "help,h", "Display this help message");
    argParser->addArgument(boost::program_options::value<bool>(&startAnim)->default_value(false)->implicit_value(true), "start,a", "start the animation loop");
    argParser->addArgument(boost::program_options::value<bool>(&printFactory)->default_value(false)->implicit_value(true), "factory,p", "print factory logs");
    argParser->addArgument(boost::program_options::value<bool>(&loadRecent)->default_value(false)->implicit_value(true), "recent,r", "load most recently opened file");
    argParser->addArgument(boost::program_options::value<bool>(&temporaryFile)->default_value(false)->implicit_value(true), "tmp", "the loaded scene won't appear in history of opened files");
    argParser->addArgument(boost::program_options::value<std::string>(&colorsStatus)->default_value("auto")->implicit_value("yes"), "colors,c", "use colors on stdout and stderr (yes, no, auto)");
    argParser->addArgument(boost::program_options::value<std::string>(&messageHandler)->default_value("auto"), "formatting,f", "select the message formatting to use (auto, clang, sofa, rich, test)");
    argParser->addArgument(boost::program_options::value<std::string>(&gui)->default_value(""), "gui,g", gui_help.c_str());
    argParser->addArgument(boost::program_options::value<bool>(&noAutoloadPlugins)->default_value(false)->implicit_value(true), "noautoload", "disable plugins autoloading");

    argParser->parse();
    files = argParser->getInputFileList();

    if (showHelp)
    {
        argParser->showHelp();
        exit(EXIT_SUCCESS);
    }

    // Note that initializations must be done after ArgumentParser that can exit the application (without cleanup)
    // even if everything is ok e.g. asking for help
    sofa::simulation::graph::init();
    sofa::component::initSofaBase();
    sofa::component::initSofaCommon();
    sofa::component::initSofaGeneral();
    
    glutInit(&argc, argv);

    sofa::simulation::setSimulation(new DAGSimulation());

    if (colorsStatus == "unset") {
        // If the parameter is unset, check the environment variable
        const char * colorStatusEnvironment = std::getenv("SOFA_COLOR_TERMINAL");
        if (colorStatusEnvironment != nullptr) {
            const std::string status (colorStatusEnvironment);
            if (status == "yes" || status == "on" || status == "always")
                sofa::helper::console::setStatus(sofa::helper::console::Status::On);
            else if (status == "no" || status == "off" || status == "never")
                sofa::helper::console::setStatus(sofa::helper::console::Status::Off);
            else
                sofa::helper::console::setStatus(sofa::helper::console::Status::Auto);
        }
    } else if (colorsStatus == "auto")
        sofa::helper::console::setStatus(sofa::helper::console::Status::Auto);
    else if (colorsStatus == "yes")
        sofa::helper::console::setStatus(sofa::helper::console::Status::On);
    else if (colorsStatus == "no")
        sofa::helper::console::setStatus(sofa::helper::console::Status::Off);

    //TODO(dmarchal): Use smart pointer there to avoid memory leaks !!
    if (messageHandler == "auto" )
    {
        MessageDispatcher::clearHandlers() ;
        MessageDispatcher::addHandler( new ConsoleMessageHandler() ) ;
    }
    else if (messageHandler == "clang")
    {
        MessageDispatcher::clearHandlers() ;
        MessageDispatcher::addHandler( new ClangMessageHandler() ) ;
    }
    else if (messageHandler == "sofa")
    {
        MessageDispatcher::clearHandlers() ;
        MessageDispatcher::addHandler( new ConsoleMessageHandler() ) ;
    }
    else if (messageHandler == "rich")
    {
        MessageDispatcher::clearHandlers() ;
        MessageDispatcher::addHandler( new ConsoleMessageHandler(&RichConsoleStyleMessageFormatter::getInstance()) ) ;
    }
    else if (messageHandler == "test"){
        MessageDispatcher::addHandler( new ExceptionMessageHandler() ) ;
    }
    else{
        msg_warning("") << "Invalid argument '" << messageHandler << "' for '--formatting'";
    }
    MessageDispatcher::addHandler(&MainPerComponentLoggingMessageHandler::getInstance()) ;

    // Initialise paths
    BaseGUI::setConfigDirectoryPath(Utils::getSofaPathPrefix() + "/config", true);
    BaseGUI::setScreenshotDirectoryPath(Utils::getSofaPathPrefix() + "/screenshots", true);

    if (!files.empty())
        fileName = files[0];

    for (unsigned int i=0; i<plugins.size(); i++)
        PluginManager::getInstance().loadPlugin(plugins[i]);

    std::string configPluginPath = PluginRepository.getFirstPath() + "/" + sofa_tostring(CONFIG_PLUGIN_FILENAME) ;
    std::string defaultConfigPluginPath = PluginRepository.getFirstPath() + "/" + sofa_tostring(DEFAULT_CONFIG_PLUGIN_FILENAME);

    if (!noAutoloadPlugins)
    {
        if (DataRepository.findFile(configPluginPath))
        {
            msg_info("runSofa") << "Loading automatically custom plugin list from " << configPluginPath;
            PluginManager::getInstance().readFromIniFile(configPluginPath);
        }
        else if (DataRepository.findFile(defaultConfigPluginPath))
        {
            msg_info("runSofa") << "Loading automatically default plugin list from " << defaultConfigPluginPath;
            PluginManager::getInstance().readFromIniFile(defaultConfigPluginPath);
        }
        else
            msg_info("runSofa") << "No plugin will be automatically loaded" << msgendl
                                << "- No custom list found at " << configPluginPath << msgendl
                                << "- No default list found at " << defaultConfigPluginPath;
    }
    else
        msg_info("runSofa") << "Automatic plugin loading disabled.";

    PluginManager::getInstance().init();

    if (int err = GUIManager::Init(argv[0], gui.c_str()))
        return err;

    if (fileName.empty())
    {
        if (loadRecent) // try to reload the latest scene
        {
            string scenes = BaseGUI::getConfigDirectoryPath() + "/runSofa.ini";
            std::ifstream mrulist(scenes.c_str());
            std::getline(mrulist,fileName);
            mrulist.close();
        }
        else
            fileName = "Demos/caduceus.scn";

        fileName = DataRepository.getFile(fileName);
    }


    if (int err=GUIManager::createGUI(NULL))
        return err;

    //To set a specific resolution for the viewer, use the component ViewerSetting in you scene graph
    GUIManager::SetDimension(800,600);

    Node::SPtr groot = sofa::simulation::getSimulation()->load(fileName.c_str());
    if( !groot )
        groot = sofa::simulation::getSimulation()->createNewGraph("");


    sofa::simulation::getSimulation()->init(groot.get());

    GUIManager::SetScene(groot,fileName.c_str(), temporaryFile);


    //=======================================
    //Apply Options

    if (startAnim)
        groot->setAnimate(true);
    if (printFactory)
    {
        msg_info("") << "////////// FACTORY //////////" ;
        sofa::helper::printFactoryLog();
        msg_info("") << "//////// END FACTORY ////////" ;
    }

    //=======================================
    // Run the main loop
    if (int err = GUIManager::MainLoop(groot,fileName.c_str()))
        return err;
    groot = dynamic_cast<Node*>( GUIManager::CurrentSimulation() );

    if (groot!=NULL)
        sofa::simulation::getSimulation()->unload(groot);

    sofa::simulation::common::cleanup();
    sofa::simulation::graph::cleanup();
    return 0;
}

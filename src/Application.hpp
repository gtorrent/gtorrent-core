#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include <memory>

class Application;	typedef std::shared_ptr<Application> app_ptr;

class Application
{
private:
	static app_ptr m_App;
public:
	Application();
	static app_ptr getSingleton();

	/* Member Variables & Functions */
	int run(int argc, char **argv);

};
#endif
#include "Config.hpp"

// Constructor
Config::Config()
{
	{
		t_error_page	page1;
		t_error_page	page2;
		t_error_page	page3;
		t_location		location1;
		t_location		location2;
		
		page1.n_error = 404;
		page1.path = "./404.html";
		page2.n_error = 405;
		page2.path = "./405.html";
		page3.n_error = 406;
		page3.path = "./406.html";

		location1.method.push_back(GET);
		location1.path = "/";
		location1.root = "/www";
		location1.index = "./homepage.html";
		
		location2.method.push_back(POST);
		location2.path = "/upload";
		location2.upload_enable = true;
		location2.upload_path = "/tmp/uploads/";

		this->port = 8080;
		this->host = "0.0.0.0";
		this->server_name.push_back("localhost");
		this->client_max_body_size = 500000;
		this->error_page.push_back(page1);
		this->error_page.push_back(page2);
		this->error_page.push_back(page3);
		this->location.push_back(location1);
		this->location.push_back(location2);

	}
}

// Destructor
Config::~Config()
{

}
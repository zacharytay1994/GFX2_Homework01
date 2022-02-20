/*
* @author:	Zachary Tay
* @date:	02/02/21
* @brief:	vulkan hw1
*/

#include <vulkan/vulkan.h>
#include <cstring>
#include <iostream>

#include "src/internal/vkHelper.h"

int main ( int argc , char* argv[] )
{
	bool enable_validation_ { false };
	bool enable_renderdoc_ { false };

	for ( int i = 0; i < argc; ++i )
	{
		if ( !strcmp ( argv[ i ] , "-d" ) )
		{
			enable_validation_ = true;
		}
		else if ( !strcmp ( argv[ i ] , "-r" ) )
		{
			enable_renderdoc_ = true;
		}
	}

	char ans;
	std::cout << "Validation layers? (Y/N)" << std::endl;
	std::cin >> ans;
	if ( ans == 'Y' || ans == 'y' )
	{
		enable_validation_ = true;
	}
	else
	{
		enable_validation_ = false;
	}

	std::cout << "RenderDoc? (Y/N)" << std::endl;
	std::cin >> ans;
	if ( ans == 'Y' || ans == 'y' )
	{
		enable_renderdoc_ = true;
	}
	else
	{
		enable_renderdoc_ = false;
	}

	VkInstance vk_instance { nullptr };
	VkDebugUtilsMessengerEXT vk_debug_messenger { nullptr };

	vk_instance = vkHelper::Create::vkInstance ( "Homework1" , enable_validation_ , enable_renderdoc_ );
	if ( enable_validation_ )
		vk_debug_messenger = vkHelper::Create::vkDebugMessenger ( vk_instance );

	std::cout << "### Setup complete.\n### Press any key to continue!" << std::endl;
	int i;
	std::cin >> i;

	return 1;
}
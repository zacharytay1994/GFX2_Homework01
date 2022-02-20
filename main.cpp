/*
* @author:	Zachary Tay
* @date:	02/02/21
* @brief:	vulkan hw1
*/

#include <vulkan/vulkan.h>
#include <cstring>
#include <iostream>
#include <exception>

#include "src/internal/vkHelper.h"
#include "src/internal/wndHelper.h"

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

	// create vulkan instance
	VkInstance vk_instance { nullptr };
	int flags { 0 };
	if ( enable_validation_ )
	{
		flags |= static_cast< int >( vkHelper::Get::VKLAYER::KHRONOS_VALIDATION );
	}
	if ( enable_renderdoc_ )
	{
		flags |= static_cast< int >( vkHelper::Get::VKLAYER::RENDERDOC_CAPTURE );
	}
	if ( !vkHelper::Create::vkInstance ( "Homework1" , vk_instance , flags ) )
	{
		throw std::runtime_error ( "Failed to create vkinstance!" );
	}
	std::cout << "### VkInstance created successfully." << std::endl;

	// create debug messenger
	VkDebugUtilsMessengerEXT vk_debug_messenger { VK_NULL_HANDLE };
	if ( enable_validation_ )
	{
		if ( !vkHelper::Create::vkDebugMessenger ( vk_instance , vk_debug_messenger ) )
		{
			throw std::runtime_error ( "Failed to create debug messenger!" );
		}
	}
	std::cout << "### VkDebugMessenger created successfully." << std::endl;

	// create window
	wndHelper::Window window;
	if ( !window.Initialize ( { 500, 500, false } ) )
	{
		throw std::runtime_error ( "Failed to initialize window!" );
	}
	std::cout << "### Window created successfully." << std::endl;

	// create surface
	VkSurfaceKHR vk_surface { VK_NULL_HANDLE };
	if ( !vkHelper::Create::vkSurfaceWin32 ( vk_instance , window.GetHandle () , vk_surface ) )
	{
		throw std::runtime_error ( "Failed to create VkSurfaceKHR!" );
	}
	std::cout << "### VkSurface created successfully." << std::endl;

	// create physical device
	VkPhysicalDevice vk_physical_device { VK_NULL_HANDLE };
	if ( ( vk_physical_device = vkHelper::Create::vkPhysicalDevice ( vk_instance , vk_surface ) ) == VK_NULL_HANDLE )
	{
		throw std::runtime_error ( "Failed to create VkPhysicalDevice!" );
	}
	std::cout << "### VkPhysicalDevice created successfully." << std::endl;

	// create logical device
	VkDevice vk_logical_device { VK_NULL_HANDLE };
	if ( ( vk_logical_device = vkHelper::Create::vkLogicalDevice ( vk_physical_device , vk_surface , flags ) ) == VK_NULL_HANDLE )
	{
		throw std::runtime_error ( "Failed to create VkDevice logical!" );
	}
	std::cout << "### VkDevice logical created successfully." << std::endl;

	// create graphics queue
	VkQueue vk_graphics_queue { VK_NULL_HANDLE };
	if ( ( vk_graphics_queue = vkHelper::Create::vkGraphicsQueue ( vk_physical_device , vk_surface , vk_logical_device ) ) == VK_NULL_HANDLE )
	{
		throw std::runtime_error ( "Failed to create graphics queue!" );
	}
	std::cout << "### VkQueue graphics created successfully." << std::endl;

	// create present queue
	VkQueue vk_present_queue { VK_NULL_HANDLE };
	if ( ( vk_present_queue = vkHelper::Create::vkPresentQueue ( vk_physical_device , vk_surface , vk_logical_device ) ) == VK_NULL_HANDLE )
	{
		throw std::runtime_error ( "Failed to create present queue!" );
	}
	std::cout << "### VkQueue present created successfully." << std::endl;

	// create swap chain
	vkHelper::vkSwapChainData vk_swapchain_data;
	if ( ( vk_swapchain_data = vkHelper::Create::vkSwapChain ( vk_physical_device , vk_surface , vk_logical_device ) ).swapchain_ == VK_NULL_HANDLE )
	{
		throw std::runtime_error ( "Failed to create VkSwapchain" );
	}
	std::cout << "### VkSwapchain created successfully." << std::endl;

	// create render pass
	VkRenderPass vk_render_pass { VK_NULL_HANDLE };
	if ( ( vk_render_pass = vkHelper::Create::vkRenderPass ( vk_logical_device , vk_swapchain_data.format_ ) ) == VK_NULL_HANDLE )
	{
		throw std::runtime_error ( "Failed to create VkRenderPass" );
	}
	std::cout << "### VkRenderPass created successfully." << std::endl;

	// create 

	std::cout << "### Setup complete.\n### Press any key to continue!" << std::endl;
	window.Update ();

	return 1;
}
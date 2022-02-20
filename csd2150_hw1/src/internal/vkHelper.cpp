#include "vkHelper.h"

#include <iostream>

namespace vkHelper
{
	static constexpr char const* VK_LAYER_VALIDATION = "VK_LAYER_KHRONOS_validation";
	static constexpr char const* VK_LAYER_RENDERDOC = "VK_LAYER_RENDERDOC_Capture";

	namespace Create
	{
		VkInstance vkInstance ( char const* name , bool enableValidation , bool enableRenderdoc )
		{
			// check validation layers and extensions
			std::vector<const char*> validation_layers;
			std::vector<char const*> extensions;

			if ( enableValidation )
			{
				validation_layers.emplace_back ( VK_LAYER_VALIDATION );
				extensions.emplace_back ( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );

				if ( enableRenderdoc )
				{
					validation_layers.emplace_back ( VK_LAYER_RENDERDOC );
				}
			}

			// check validation layer support
			if ( !Check::ValidationLayersSupport ( validation_layers ) )
			{
				std::cerr << "### Create::vkInstance failed! Validation layers requested not supported!" << std::endl;
			}

			// check extension support
			if ( !Check::ExtensionsSupport ( extensions ) )
			{
				std::cerr << "### Create::vkInstance failed! Extensions requested not supported!" << std::endl;
			}

			// app info
			VkApplicationInfo app_info {};
			app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			app_info.pApplicationName = name;
			app_info.applicationVersion = VK_MAKE_VERSION ( 1 , 0 , 0 );
			app_info.pEngineName = "Engine";
			app_info.engineVersion = VK_MAKE_VERSION ( 1 , 0 , 0 );
			app_info.apiVersion = VK_API_VERSION_1_0;

			// create info for app info
			VkInstanceCreateInfo create_info {};
			create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			create_info.pApplicationInfo = &app_info;

			// validation layer settings
			create_info.enabledLayerCount = static_cast< uint32_t >( validation_layers.size () );
			create_info.ppEnabledLayerNames = validation_layers.size () > 0 ? validation_layers.data () : nullptr;

			create_info.enabledExtensionCount = static_cast< uint32_t >( extensions.size () );
			create_info.ppEnabledExtensionNames = extensions.size() > 0 ? extensions.data () : nullptr;

			create_info.pNext = nullptr;

			if ( enableValidation )
			{
				// set debug info
				VkDebugUtilsMessengerCreateInfoEXT debug_create_info {};
				Debug::PopulateDebugMessengerCreateInfo ( debug_create_info );
				create_info.pNext = ( VkDebugUtilsMessengerCreateInfoEXT* ) &debug_create_info;
			}

			VkInstance instance { nullptr };
			if ( vkCreateInstance ( &create_info , nullptr , &instance ) != VK_SUCCESS )
			{
				std::cerr << "### Create::vkInstance failed to create VkInstance!" << std::endl;
				return nullptr;
			}

			return instance;
		}

		VkDebugUtilsMessengerEXT vkDebugMessenger ( VkInstance instance )
		{
			VkDebugUtilsMessengerCreateInfoEXT debug_create_info {};
			Debug::PopulateDebugMessengerCreateInfo ( debug_create_info );

			VkDebugUtilsMessengerEXT debug_messenger;
			if ( Debug::CreateDebugUtilsMessengerEXT ( instance , &debug_create_info , nullptr , &debug_messenger ) != VK_SUCCESS )
			{
				std::cerr << "### Create::vkDebugMessenger Failed to set up debug messenger!" << std::endl;
			}

			return debug_messenger;
		}
	}

	namespace Check
	{
		bool ValidationLayersSupport ( std::vector<char const*> requestedLayers )
		{
			std::cout << "### Requested Layers" << std::endl;
			for ( const auto& layer : requestedLayers )
			{
				std::cout << "\t- " << layer << std::endl;
			}

			// get available layers
			uint32_t layer_count;
			vkEnumerateInstanceLayerProperties ( &layer_count , nullptr );
			std::vector<VkLayerProperties> available_layers ( layer_count );
			vkEnumerateInstanceLayerProperties ( &layer_count , available_layers.data () );

			std::cout << "### Available Layers" << std::endl;;
			for ( const auto& layer : available_layers )
			{
				std::cout << "\t- " << layer.layerName << std::endl;
			}

			for ( const auto& requested_layer : requestedLayers )
			{
				bool layer_found = false;
				for ( const auto& layerProperty : available_layers )
				{
					if ( !strcmp ( requested_layer , layerProperty.layerName ) )
					{
						layer_found = true;
					}
				}
				if ( !layer_found )
				{
					std::cerr << "### Requested layer not supported." << std::endl;
					std::cerr << "###\t- " << requested_layer << std::endl;
					return false;
				}
			}

			std::cout << "### All requested layers supported!" << std::endl;
			return true;
		}

		bool ExtensionsSupport ( std::vector<char const*> requestedExtensions )
		{
			// get available extensions
			uint32_t extension_count { 0 };
			//  - get number of supported extensions
			vkEnumerateInstanceExtensionProperties ( nullptr , &extension_count , nullptr );
			//  - allocate container for storing the extensions
			std::vector<VkExtensionProperties> vulkan_extensions ( extension_count );
			//  - query supported extensions details
			vkEnumerateInstanceExtensionProperties ( nullptr , &extension_count , vulkan_extensions.data () );

			// print out requested extensions
			std::cout << "### Requested Extensions" << std::endl;
			for ( const auto& extension : requestedExtensions )
			{
				std::cout << "\t- " << extension << std::endl;
			}

			// print out available extensions provided by vulkan
			std::cout << "### Available Extensions" << std::endl;
			for ( const auto& extension : vulkan_extensions )
			{
				std::cout << "\t- " << extension.extensionName << std::endl;
			}

			// check if all extensions are supported by vulkan
			for ( const auto& extension : requestedExtensions )
			{
				bool extension_found { false };
				for ( auto& vulkan_extension : vulkan_extensions )
				{
					if ( strcmp ( vulkan_extension.extensionName , extension ) )
					{
						extension_found = true;
					}
				}
				if ( !extension_found )
				{
					std::cerr << "### Requested extension not supported." << std::endl;
					std::cerr << "###\t- " << extension << std::endl;
					return false;
				}
			}
			std::cout << "### All requested extensions supported." << std::endl;
			return true;
		}
	}

	namespace Debug
	{
#define JZVK_ALL_LAYER_MESSAGES
		VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback ( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity , VkDebugUtilsMessageTypeFlagsEXT messageType , const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData , void* pUserData )
		{
			if ( messageSeverity & VK_DEBUG_REPORT_ERROR_BIT_EXT )
			{
				std::cerr << "[ERROR]\n"
					<< "\t[CODE: " << pCallbackData->messageIdNumber << "]\n"
					<< "\t[MESSAGE: " << pCallbackData->pMessage << "]" << std::endl;
			}
			else if ( messageSeverity & VK_DEBUG_REPORT_WARNING_BIT_EXT )
			{
				std::cerr << "[WARNING]\n"
					<< "\t[CODE: " << pCallbackData->messageIdNumber << "]\n"
					<< "\t[MESSAGE: " << pCallbackData->pMessage << "]" << std::endl;
			}
			else
			{
#ifdef JZVK_ALL_LAYER_MESSAGES
				std::cerr << "[INFO]\n"
					<< "\t[CODE: " << pCallbackData->messageIdNumber << "]\n"
					<< "\t[MESSAGE: " << pCallbackData->pMessage << "]" << std::endl;
#endif
			}
			// should always return false, i.e. not abort function call that triggered this callback
			return VK_FALSE;
		}

		void PopulateDebugMessengerCreateInfo ( VkDebugUtilsMessengerCreateInfoEXT& createInfo )
		{
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			// flag possible problems
			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

			// debug call back function defined above 
			createInfo.pfnUserCallback = DebugCallback;

			createInfo.pUserData = nullptr;
		}

		VkResult CreateDebugUtilsMessengerEXT ( VkInstance instance , const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo , const VkAllocationCallbacks* pAllocator , VkDebugUtilsMessengerEXT* pDebugMessenger )
		{
			auto func = ( PFN_vkCreateDebugUtilsMessengerEXT ) vkGetInstanceProcAddr ( instance , "vkCreateDebugUtilsMessengerEXT" );
			if ( func != nullptr )
			{
				return func ( instance , pCreateInfo , pAllocator , pDebugMessenger );
			}
			else
			{
				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
		}

		void DestroyDebugUtilsMessengerEXT ( VkInstance instance , VkDebugUtilsMessengerEXT debugMessenger , const VkAllocationCallbacks* pAllocator )
		{
			auto func = ( PFN_vkDestroyDebugUtilsMessengerEXT ) vkGetInstanceProcAddr ( instance , "vkDestroyDebugUtilsMessengerEXT" );
			if ( func != nullptr )
			{
				func ( instance , debugMessenger , pAllocator );
			}
			else
			{
				std::cerr << "### Debug::DestroyUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT func not loaded!" << std::endl;
			}
		}
	}
}
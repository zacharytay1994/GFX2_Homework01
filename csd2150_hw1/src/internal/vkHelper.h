#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace vkHelper
{
	namespace Create
	{
		VkInstance vkInstance ( char const* name , bool enableValidation , bool enableRenderdoc );

		VkDebugUtilsMessengerEXT vkDebugMessenger ( VkInstance instance );
	}

	namespace Check
	{
		bool ValidationLayersSupport ( std::vector<char const*> requestedLayers );

		bool ExtensionsSupport ( std::vector<char const*> requestedExtensions );
	}

	namespace Debug
	{
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback ( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity ,
			VkDebugUtilsMessageTypeFlagsEXT messageType ,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData ,
			void* pUserData );

		void PopulateDebugMessengerCreateInfo ( VkDebugUtilsMessengerCreateInfoEXT& createInfo );

		VkResult CreateDebugUtilsMessengerEXT ( VkInstance instance ,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo ,
			const VkAllocationCallbacks* pAllocator ,
			VkDebugUtilsMessengerEXT* pDebugMessenger );

		void DestroyDebugUtilsMessengerEXT ( VkInstance instance ,
			VkDebugUtilsMessengerEXT debugMessenger ,
			const VkAllocationCallbacks* pAllocator );
	}
}
#pragma once
#define VK_USE_PLATFORM_WIN32_KHR 1
#include <vulkan/vulkan.h>
#include <vector>
#include <Windows.h>
#include <optional>
#include <array>
#include <unordered_map>

namespace vkHelper
{
	struct vkSwapChainData
	{
		VkSwapchainKHR				swapchain_ { VK_NULL_HANDLE };
		VkExtent2D					extent_;
		VkFormat					format_;
		std::vector<VkImage>		images_;
		std::vector<VkImageView>	image_views_;
	};

	struct vkPipelineData
	{
		VkPipeline			pipeline_;
		VkPipelineLayout	layout_;
	};

	struct vkSyncObjects
	{
		std::vector<VkSemaphore>	available_semaphores_;
		std::vector<VkSemaphore>	finished_semaphores_;
		std::vector<VkFence>		in_flight_fences_;
		std::vector<VkFence>		images_in_flight_;
	};

	namespace Create
	{
		bool vkInstance ( char const* name , VkInstance& instance , int flags );

		bool vkDebugMessenger ( VkInstance instance , VkDebugUtilsMessengerEXT& debugMessenger );

		bool vkSurfaceWin32 ( VkInstance instance , HWND hWnd , VkSurfaceKHR& surface );

		VkPhysicalDevice	vkPhysicalDevice ( VkInstance instance , VkSurfaceKHR surface );

		VkDevice			vkLogicalDevice ( VkPhysicalDevice physicalDevice , VkSurfaceKHR surface , int flags );

		VkQueue				vkGraphicsQueue ( VkPhysicalDevice physicalDevice , VkSurfaceKHR surface , VkDevice logicalDevice );

		VkQueue				vkPresentQueue ( VkPhysicalDevice physicalDevice , VkSurfaceKHR surface , VkDevice logicalDevice );

		vkSwapChainData		vkSwapChain ( VkPhysicalDevice physicalDevice , VkSurfaceKHR surface , VkDevice logicalDevice );

		VkRenderPass		vkRenderPass ( VkDevice logicalDevice , VkFormat imageFormat );

		vkPipelineData		vkGraphicsPipeline ( VkDevice logicalDevice , vkSwapChainData swapChainData , VkRenderPass renderPass );

		bool vkFramebuffers ( VkDevice logicalDevice , vkSwapChainData& swapChainData , VkRenderPass renderPass , std::vector<VkFramebuffer>& framebuffers );

		VkCommandPool vkCommandPool ( VkPhysicalDevice physicalDevice , VkSurfaceKHR surface , VkDevice logicalDevice );

		bool vkCommandBuffers ( VkDevice logicalDevice , vkSwapChainData swapChain , VkRenderPass renderPass , vkPipelineData graphicsPipeline , std::vector<VkFramebuffer>& framebuffers , VkCommandPool commandPool , std::vector<VkCommandBuffer>& commandBuffers );

		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
		bool SyncObjects ( VkDevice logicalDevice , vkSwapChainData swapChain , vkSyncObjects& syncObjects );
	}

	namespace Check
	{
		bool vkLayersSupport ( std::vector<char const*> requestedLayers );

		bool InstanceExtensionsSupport ( bool debug );

		bool DeviceExtensionsSupport ( VkPhysicalDevice device );

		bool SwapChainSupport ( VkPhysicalDevice physicalDevice , VkSurfaceKHR surface );

		bool PhysicalDeviceSuitable ( VkPhysicalDevice device , VkSurfaceKHR surface );
	}

	namespace Get
	{
		enum class VKLAYER
		{
			KHRONOS_VALIDATION = 1 ,
			RENDERDOC_CAPTURE = 2
		};

		static std::unordered_map<VKLAYER , char const*> vk_layers_
		{
			{ VKLAYER::KHRONOS_VALIDATION , "VK_LAYER_KHRONOS_validation" },
			{ VKLAYER::RENDERDOC_CAPTURE , "VK_LAYER_RENDERDOC_Capture" }
		};

		char const* vkLayer ( VKLAYER layer );

		std::vector<const char*> ValidationLayers ( int flags );

		std::vector<char const*> InstanceExtensions ( bool debug );

		std::vector<char const*> DeviceExtensions ();

		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphics_family_;
			std::optional<uint32_t> present_family_;

			bool IsComplete ()
			{
				return graphics_family_.has_value ()
					&& present_family_.has_value ();
			}
		};
		QueueFamilyIndices QueueFamilies ( VkPhysicalDevice physicalDevice , VkSurfaceKHR surface );

		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities_;
			std::vector<VkSurfaceFormatKHR> formats_;
			std::vector<VkPresentModeKHR> present_modes_;
		};

		// swap chain
		SwapChainSupportDetails SwapChainSupportDetails_f ( VkPhysicalDevice physicalDevice , VkSurfaceKHR surface );

		VkSurfaceFormatKHR vkSwapChainSurfaceFormat ( VkPhysicalDevice physicalDevice , VkSurfaceKHR surface );

		VkPresentModeKHR vkSwapChainPresentMode ( VkPhysicalDevice physicalDevice , VkSurfaceKHR surface );

		VkExtent2D vkSwapChainExtent2D ( VkPhysicalDevice physicalDevice , VkSurfaceKHR surface );

		std::vector<VkImage> vkSwapChainImages ( VkDevice logicalDevice , VkSwapchainKHR swapChain );

		std::vector<VkImageView> vkSwapChainImageViews ( VkDevice logicalDevice , std::vector<VkImage> const& swapChainImages , VkFormat swapChainImageFormat );
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

	namespace IO
	{
		std::vector<char> ReadFile ( std::string const& filename );
		VkShaderModule CreateShaderModule ( VkDevice logicalDevice , std::vector<char> const& code );
	}

	namespace Misc
	{
		void DrawFrame ( VkDevice logicalDevice , VkQueue graphicsQueue , VkQueue presentQueue , vkSwapChainData& swapChain , std::vector<VkCommandBuffer>& commandBuffers , vkSyncObjects& syncObjects , size_t& currentFrame );
	}
}
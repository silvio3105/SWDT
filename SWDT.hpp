/**
 * @file sWDT.h
 * @author silvio3105 (www.github.com/silvio3105)
 * @brief Watchdog header-only driver file.
 * 
 * @copyright Copyright (c) 2023, silvio3105
 * 
 */

/*
License

Copyright (c) 2023, silvio3105 (www.github.com/silvio3105)

Access and use of this Project and its contents are granted free of charge to any Person.
The Person is allowed to copy, modify and use The Project and its contents only for non-commercial use.
Commercial use of this Project and its contents is prohibited.
Modifying this License and/or sublicensing is prohibited.

THE PROJECT AND ITS CONTENT ARE PROVIDED "AS IS" WITH ALL FAULTS AND WITHOUT EXPRESSED OR IMPLIED WARRANTY.
THE AUTHOR KEEPS ALL RIGHTS TO CHANGE OR REMOVE THE CONTENTS OF THIS PROJECT WITHOUT PREVIOUS NOTICE.
THE AUTHOR IS NOT RESPONSIBLE FOR DAMAGE OF ANY KIND OR LIABILITY CAUSED BY USING THE CONTENTS OF THIS PROJECT.

This License shall be included in all methodal textual files.
*/


#ifndef _SWDT_H_
#define _SWDT_H_

// ----- INCLUDE FILES
#include			MCU_FILE
#include			"SML.hpp"
#include			<stdint.h>


/** \addtogroup SWDT
 * @{
 * Simple watchdog driver for STM32.
*/

// ----- DEFINES
#define SWDT_VERSION			"v1.0r1" /**< @brief Driver version string. */


// ----- NAMESPACES
/**
 * @brief Namespace for simple watchdog driver.
 * 
 */
namespace SWDT
{
	// MAIN CLASS
	/**
	 * @brief Simple watchdog driver API.
	 * 
	 * @tparam C Watchdog class. \ref SWDT_IWDG or \ref SWDT_WWDG
	 */
	template<class C>
	class SWDT
	{
		public:
		/**
		 * @brief Watchdog object constructor.
		 * 
		 * @param timeout Watchdog timeout in ms.
		 * @return No return value.
		 */
		SWDT(uint32_t timeout)
		{
			static_cast<C*>(this)->init();
			setTimeout(timeout);
		}

		/**
		 * @brief Watchdog object deconstructor.
		 * 
		 * @return No return value.
		 */
		~SWDT(void)
		{	
			freq = 1;
		}

		/**
		 * @brief Start watchdog.
		 * 
		 * @return No return value.
		 */
		inline void start()
		{
			static_cast<C*>(this)->start();
		}

		/**
		 * @brief Reload watchdog.
		 * 
		 * @return No return value.
		 */
		inline void feed(void)
		{
			static_cast<C*>(this)->feed();
		}

		/**
		 * @brief Set watchdog timeout in ms.
		 * 
		 * @param timeout Watchdog timeout in ms.
		 * @return No return value.
		 */
		inline void setTimeout(uint32_t timeout)
		{
			static_cast<C*>(this)->setTimeout(timeout);
		}

		/**
		 * @brief Set watchdog input freqnuency in Hz.
		 * 
		 * @param value Input freqnuency in Hz.
		 * @return No return value.
		 */
		inline void setInputFreq(uint32_t value)
		{
			freq = value;
		}

		protected:
		// VARIABLES
		uint32_t freq = 1; /**< @brief Watchdog input frequency. */
	};

	// CLASS FOR STM32 IWDG
	/** @cond PRIVATE */
	/**
	 * @brief STM32 independent watchdog(IWDG) class. 
	 * 
	 */
	class SWDT_IWDG : public SWDT<SWDT_IWDG>
	{
		public:
		SWDT_IWDG(uint32_t timeout) : SWDT(timeout)
		{

		}

		~SWDT_IWDG(void)
		{

		}


		/**
		 * @brief Init IWDG object.
		 * 
		 */
		void init(void)
		{
			// Set IWDG input freq
			freq = 40000;
		}

		/**
		 * @brief Start IWDG.
		 * 
		 * @return No return value.
		 * 
		 * @note Once started, IWDG cannot be stopped.
		 */
		void start(void)
		{
			IWDG->KR = startKey;
		}

		/**
		 * @brief Reload IWDG counter
		 * 
		 * @return No return value.
		 */
		void feed(void)
		{
			IWDG->KR = reloadKey;
		}

		/**
		 * @brief Set the Timeout object
		 * 
		 * @param timeout IWDG timeout in miliseconds.
		 * @return No return value.
		 */
		void setTimeout(uint32_t timeout)
		{
			// Set prescaler to max
			setPrescaler(Prescaler_t::Div256);

			// Set required reload value
			setReloadValue(timeout / (1000 / (freq / 256)));
		}

		private:
		// CONSTANTS
		static constexpr uint16_t reloadKey = 0xAAAA; /**< @brief Reload key for IWDG. */
		static constexpr uint16_t accessKey = 0x5555; /**< @brief Access key for IWDG. */
		static constexpr uint16_t startKey = 0xCCCC; /**< @brief Start key for IWDG. */
		static constexpr uint16_t maxReloadValue = 4095; /**< @brief Maximum reload value for IWDG. */

		// ENUMS
		/**
		 * @brief IWDG input clock prescaler enum.
		 * 
		 */
		enum class Prescaler_t : uint8_t {
			Div4 = 0b000, /**< @brief IWDG clock prescaler 4. */
			Div8 = 0b001, /**< @brief IWDG clock prescaler 8. */
			Div16 = 0b010, /**< @brief IWDG clock prescaler 16. */
			Div32 = 0b011, /**< @brief IWDG clock prescaler 32. */
			Div64 = 0b100, /**< @brief IWDG clock prescaler 64. */
			Div128 = 0b101, /**< @brief IWDG clock prescaler 128. */
			Div256 = 0b110 /**< @brief IWDG clock prescaler 256. */
		};

		// METHOD DEFINITIONS
		/**
		 * @brief Enable register write access.
		 * 
		 * @return No return value.
		 */
		inline void enableAccess(void)
		{
			// Write access value to KR register to unlock register protection
			IWDG->KR = accessKey;
		}

		/**
		 * @brief Set IWDG reload value.
		 * 
		 * @param value Reload value(max. \ref maxReloadValue)
		 * @return No return value.
		 */
		void setReloadValue(uint32_t value)
		{
			// Prevent reload value to go over its maximum
			SML::limit<uint32_t>(value, 1, maxReloadValue);

			// Wait if reload value update is ongoing
		//	while (IWDG->SR & IWDG_SR_RVU);

			// Enable write access
			enableAccess();	

			// Write new reload value
			IWDG->RLR = value;
		}

		/**
		 * @brief Set IWDG input clock prescaler.
		 * 
		 * @param prescaler Prescaler value. See \ref Prescaler_t
		 */
		void setPrescaler(const Prescaler_t prescaler)
		{
			// Wait if prescaler update is ongoing
		//	while (IWDG->SR & IWDG_SR_PVU);

			// Enable write access
			enableAccess();

			// Set new prescaler
			IWDG->PR = (uint8_t)prescaler;
		}
	};

	// CLASS FOR STM32 WWDG
	class SWDT_WWDG : public SWDT<SWDT_WWDG>
	{
		public:
		SWDT_WWDG(uint32_t timeout) : SWDT(timeout)
		{

		}

		~SWDT_WWDG(void)
		{

		}

		void init(void)
		{

		}

		void start(void)
		{

		}

		void feed(void)
		{

		}

		void setTimeout(uint32_t timeout)
		{

		}
	};

	/** @endcond */
};

/**@}*/

#endif // _SWDT_H_

// END WITH NEW LINE

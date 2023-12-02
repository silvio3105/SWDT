/**
 * @file SWDT.h
 * @author silvio3105 (www.github.com/silvio3105)
 * @brief Watchdog header file.
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

#ifndef SWDT_TIMEOUT
#define SWDT_TIMEOUT			100 /**< @brief Driver operation timeout in ms. User can redefine it during build. */
#endif // SWDT_TIMEOUT


// ----- NAMESPACES
namespace SWDT
{
	// MAIN CLASS
	template<class C>
	class SWDT
	{
		public:
		SWDT(void)
		{

		}

		~SWDT(void)
		{

		}


		inline void start() const
		{
			static_cast<C>(this)->start();
		}

		inline void feed(void) const
		{
			static_cast<C>(this)->feed();
		}

		inline void setTimeout(uint32_t timeout) const
		{
			static_cast<C>(this)->setTimeout(timeout);
		}

		inline void setInputFreq(uint32_t value)
		{
			static_cast<C>(this)->setInputFreq(value);
		}
	};

	// CLASS FOR STM32 IWDG
	class IWDG : SWDT<IWDG>
	{
		public:
		IWDG(void)
		{

		}

		~IWDG(void)
		{

		}


		void start(void) const
		{
			IWDG->KR = startKey;
		}

		void feed(void) const
		{
			IWDG->KR = reloadKey;
		}

		void setTimeout(uint32_t timeout) const
		{
			// Set prescaler to max
			setPrescaler(Prescaler_t::Div256);

			// Set required reload value
			setReloadValue(timeout / (1000 / (freq / 256)));

		}

		void setInputFreq(uint32_t value)
		{
			freq = value;
		}


		private:
		// CONSTANTS
		constexpr uint16_t reloadKey = 0xAAAA; /**< @brief Reload key for IWDG. */
		constexpr uint16_t accessKey = 0x5555; /**< @brief Access key for IWDG. */
		constexpr uint16_t startKey = 0xCCCC; /**< @brief Start key for IWDG. */
		constexpr uint16_t maxReloadValue = 4095; /**< @brief Maximum reload value for IWDG. */

		// ENUMS
		enum class Prescaler_t : uint8_t {
			Div4 = 0b000, /**< @brief IWDG clock prescaler 4. */
			Div8 = 0b001, /**< @brief IWDG clock prescaler 8. */
			Div16 = 0b010, /**< @brief IWDG clock prescaler 16. */
			Div32 = 0b011, /**< @brief IWDG clock prescaler 32. */
			Div64 = 0b100, /**< @brief IWDG clock prescaler 64. */
			Div128 = 0b101, /**< @brief IWDG clock prescaler 128. */
			Div256 = 0b110 /**< @brief IWDG clock prescaler 256. */
		};

		// VARIABLES
		uint32_t freq = 32000; /**< @brief IWDG input clock freq. */

		// METHOD DEFINITIONS
		/**
		 * @brief Enable register write access.
		 * 
		 * @return No return value.
		 */
		inline void enableAccess(void) const
		{
			// Write access value to KR register to unlock register protection
			IWDG->KR = accessKey;
		}

		void setReloadValue(uint32_t value) const
		{
			if (value > maxReloadValue)
			{
				value = maxReloadValue;
			}

			// Write new reload value
			IWDG->RLR = value;
		}

		void setPrescaler(const Prescaler_t prescaler) const
		{
			// Wait if prescaler update is ongoing
			while (IWDG->SR & IWDG_SR_PVU);

			// Enable write access
			enableAccess();

			// Set new prescaler
			IWDG->PR = (uint8_t)prescaler;

			// Feed watchdog
			feed();
		}
	};

	// CLASS FOR STM32 WWDG
	class WWDG : SWDT<WWDG>
	{
		public:
		WWDG(void)
		{

		}

		~WWDG(void)
		{

		}

		void start(void) const
		{

		}

		void feed(void) const
		{

		}

		void setTimeout(uint32_t timeout) const
		{

		}

		void setInputFreq(uint32_t value)
		{

		}		

		private:
	};
};

/**@}*/

#endif // _SWDT_H_

// END WITH NEW LINE

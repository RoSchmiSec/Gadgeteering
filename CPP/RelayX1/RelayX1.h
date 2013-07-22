/*
Copyright 2013 GHI Electronics LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "../Gadgeteering/Interfaces.hpp"
#include "../Gadgeteering/Mainboard.hpp"
#include "../Gadgeteering/System.hpp"
#include "../Gadgeteering/Types.hpp"

namespace GHI
{
	namespace Modules
	{
		class RelayX1 : protected Module
		{
			protected:
				Interfaces::DigitalOutput *output;
				bool enabled;

			public:
				RelayX1(int socket);

				void EnableRelay();
				void DisableRelay();

				bool RelayState();
		};
	}
}
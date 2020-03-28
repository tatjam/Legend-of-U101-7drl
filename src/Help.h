#pragma once
#include <string>
#include <unordered_map>


class Help
{
public:

	std::string cur_screen;

	std::unordered_map<std::string, std::string> screens;

	std::vector<std::string> return_to;

	Help()
	{
// Text template
//                                                                                     |
		screens["Main Screen"] = "\
Welcome to $174$$174$The Legend of U101$175$$175$.\n\
This is the help screen which will guide you while you play the game, it can be opened\n\
and closed with $yellow$F1$white$. \n\
Items coloured in $blue$blue$white$ are links, which can be clicked to learn further.\n\
\n\
-@Tutorial@\n\
-@Controls Overview@\n\
";

		screens["Tutorial"] = "\
You start the game inside your submarine, your vehicle has just left one of the few\n\
human bases under Jupiter-2's (Europa) ice crust, with the objective of cleaning the\n\
area for further colonization. In order to complete this objective, you must travel\n\
near nests, marked in the sonar as a $sonar$$15$$white$.\n\
Once you are sufficiently close, you will get a prompt to disembark.\n\
\n\
Before any of that happens you will have to travel through the dangerous underground\n\
icy caverns. To do so, you will need to carefully manage your crew, issuing orders and\n\
controlling the many workstations available.\n\n\
To select a crewmember $1$ ($2$ is you, the captain), $yellow$left click$white$ him. Once\n\
selected, you can $yellow$right click$white$ to bring up the contextual menu, or\n\
$yellow$middle click$white$ anywhere to move the crewmember there. See @Controlling Crew@.\n\n\
To control a workstation, left-click on it. A crewmember must occupy the chair ($brown$$239$$white$)\n\
in order for the workstation to be usable, and for orders issued to it to take place.\n\
To exit the workstation, press $yellow$Escape$white$\n\n\
@Tutorial 1: Overview of the Submarine@\n\
@Tutorial 2: Preparing@\n\
@Tutorial 3: Movement@\n\
@Tutorial 4: Combat@\n\
@Tutorial 5: Disembarking@\n\
\n\
You can pause the game anytime with $yellow$SPACE$white$ so feel free to take your time\n\
Also, check the @Controls Overview@ to find every single keybind!";

		screens["Tutorial 1: Overview of the Submarine"] = "\
The leftmost part of the submarine is the torpedo room. The small ($dark$=$white$) symbols are\n\
the torpedo tubes. You can order torpedos to be loaded by selecting a crewmember\n\
right-clicking over a torpedo, and selecting $yellow$Load Torpedo$white$.\n\n\
The next room are the living quarters.\n\n\
The next two rooms hold two very important work stations, the topmost one is the @Sonar@\n\
and the bottom one is the @Radio@\n\n\
The next room contains the @Periscope@ (center), the @Maneuvering Station@(top)\n\
and the @Targeting Station@(bottom).\n\n\
The next room contains the @Diesel Engines@.\n\n\
The last room contains the @Battery Station@ and a few extra torpedoes\n\n";
		
		screens["Tutorial 2: Preparing"] = "\
The submarine currently is nothing more that a barely floating piece of metal. You need to\n\
prepare the multiple systems before you depart. First of all, you need electricity.\n\
Head over to the @Battery Station@ and toggle on (by left-clicking) the switches:\n\
- $174$$dark$BAT.A > MOTOR$white$$175$ (Allows moving with the electric engines)\n\
- $174$$dark$BAT.B > BAT.A$white$$175$ (Uses the B battery to fuel the motor indirectly, extending range)\n\
- $174$$dark$BAT.A > B.SHK$white$$175$ (Extends usage of the electric shock weapon)\n\n\
Now you need to load some torpedos.\nLeft-click any crewmember and right-click one of the torpedos,\n\
then select $yellow$Load Torpedo$white$. Do this until all tubes are loaded.\n\
Then you will want to set atleast 2 crewmembers to Auto-Repair. The captain ($2$) is\n\
always a good option. You can also set the radio-man and the diesel engine controller.\n\
To do this, left-click a crewmember, right-click anywhere and select $yellow$Auto-Repair$white$\n\
You are now ready to proceed. Click $yellow$R$white$ to return all crew to their positions.\n\n\
You may have already received a radio message, to view it, click the @Radio@ and listen.";
		
		screens["Tutorial 3: Movement"] = "\
You can move by manually using the @Maneouvering Station@, but it's easier and faster to\n\
use the numpad keys, which allows full directional movement, including diagonals.\n\
You can issue throttle commands with the numbers on top of the keyboard.\n\n\
Head over to the sonar, and toggle on the $174$$dark$Active Sonar$white$$175$ switch.\n\
You will now see the world surrounding you, and hear sonar pings.\n\
You should toggle the sonar off if you want to carefully avoid some enemy.\n\
Now go to the @Periscope@. It has a relatively short range (~200m), but it's always on\n\
and shows the cave walls which the sonar may not warn you about. \n\
Use the @Periscope@ whenever you are maneovering on a tight passage,\n\
when approaching nests, or when fighting enemies from a close range.\n\n\
Try moving in some direction free of walls (Use the numpad) on normal throttle:\n\
- $yellow$1$white$ = Stop\n\
- $yellow$2$white$ = Slow\n\
- $yellow$3$white$ = Normal\n\
- $yellow$4$white$ = Fast\n\
- $yellow$5$white$ = Full Speed Ahead (You need two batteries assigned to MOTOR to do this)\n\
\n\n\
While you are moving, any battery assigned to MOTOR will deplete. You must be careful,\n\
if you run out of electricity you could very easily get stuck, and lose the game.\n\
In order to refill batteries you need to get to oxygen-rich zones,\n\
marked in the sonar as a ($sonar$$177$$white$). Try maneouvering to one.\n\
Once you reach the zone, stop and head over to the @Diesel Engines@. \n\
You want to set atleast one of the generators to output to BATTS (Both batteries), and enable it.\n\
You can set the other one to wathever you want.\n\
Keep in mind that running both engines makes a lot of noise and may attract nearby enemies!\n\
Feel free to start exploring the map, and keep on checking the radio!\n\n\
Reminder: You can check your coordinates on the sonar. \n\
They are called \"Coarse Coordinates\", \"Fine Coordinates\" are your sub-tile\n\
position, not world-position.";
		
		screens["Tutorial 4: Combat"] = "\
You may currently be under attack. First of all, pause the game and don't panic.\n\
If you have prepared correctly, you will have torpedos loaded and ready to fire.\n\
It may be wise to move one of the crewmembers to the torpedo room.\n\
Make sure the sonar, targeting station, maneouvering station and periscope are crewed\n\
during combat, as you will use them a lot. Make sure that the active sonar is toggled\n\
 on in order to be able to target your enemy even if you lose sight of him.\n\n\
In order to destroy your objective, go to the @Targeting Station@, left-click the target\n\
that you want to destroy, and, once it's selected, click $yellow$F$white$ to fire a torpedo.\n\
Make sure you are aiming roughly towards your target, otherwise you may miss.\n\n\
Your other weapon is the electric shock, activated with $yellow$E$white$. It consumes\n\
electricity from the SHK battery, and does a considerable ammount of damage to targets\n\
near your submarine. Useful against smaller enemies.";

		screens["Tutorial 5: Disembarking"] = "\
Once you are sufficiently close to either a Nest ($sonar$$15$$white$) or a Station ($sonar$$234$$white$)\n\
you will get a prompt to disembark on the upper-left corner of the screen.\n\
Once you click the $yellow$G$white$ key, four of your crewmembers will go inside the target.\n\n\
Your objective inside is to find and destroy the enemy nest, or the enemy \"commander\".\n\
You can select any of your crewmembers by left-clicking them. Once selected, you can:\n\
- Move: Using the $yellow$middle mouse button$white$\n\
- Change Target: Using the $yellow$right mouse button$white$\n\
- Throw Grenades: Using $yellow$SHIFT$white$ and the $right mouse button$\n\n\
When you complete your objective, move every crewmember into the $sonar$Green$white$ zone.\n\n\n\
Your crew can be killed, so be careful.\nAlso, remember you can always pause the game with $yellow$SPACE$white$!";

		screens["Controls Overview"] = "\
- $yellow$Numpad Numbers$white$: Issue heading orders\n\
- $yellow$1$white$: Stop vehicle\n\
- $yellow$2$white$: Move slowly\n\
- $yellow$3$white$: Move normally\n\
- $yellow$4$white$: move fast\n\
- $yellow$5$white$: Full speed ahead (Only possible with 2 batteries or diesel engines active)\n\
- $yellow$F$white$: Fire torpedo\n\
- $yellow$R$white$: Return all crew to assigned positions\n\
- $yellow$G$white$: Disembark if possible\n\
- $yellow$E$white$: Electric Shock\n\
- $yellow$ESCAPE$white$: Go back / Close Workbench / Close popup\n\
- $yellow$SPACE$white$: Pause the game\n\n\
-----------------------------------------------------\n\
Special controls (No numpad or middle mouse button): \n\n\
- $yellow$Vi keys$white$: Set heading\n\
- $yellow$Ctrl-Leftclick$white$: Same as middle clicking\n\n\
You don't need to enable anything, these controls are always enabled!";

		screens["Controlling Crew"] = "\
You can select crewmembers by left clicking them. Once they are selected you can bring up\n\
the context menu by right-clicking. You can also move directly by pressing the\n\
$yellow$middle mouse button$white$. In the context menu you can:\n\
- Move Here: Moves the crew to given location\n\
- Assign Location: The crew will go to selected location when $yellow$R$white$ is pressed\n\
- Load Torpedo: Only shows when you right click over a torpedo\n\
- Pump Water Out: Only shows when you right click a pump ($235$)\n\
- Repair Tile: Only shows when you right click a damaged tile\n\
- Toggle Auto-Repair: Allows the crew to automatically repair damaged tiles he detects\n\n\
";


		cur_screen = "Main Screen";

	}

	void draw()
	{
		Drawing::draw_window(TCODConsole::root, 0, 0, 89, HEIGHT - 1, cur_screen, true);

		TCODConsole::root->setDefaultForeground(TCODColor::lightGrey);
		std::string& cur = screens[cur_screen];

		int x = 1;
		int y = 1;

		TCODConsole::root->setDefaultForeground(TCODColor::white);

		TCOD_mouse_t mouse = TCODMouse::getStatus();

		if (g_key.vk == TCODK_ESCAPE)
		{
			if (return_to.size() != 0)
			{
				std::string ret = return_to[return_to.size() - 1];
				return_to.pop_back();

				cur_screen = ret;
			}
		}

		for (int i = 0; i < cur.size(); i++)
		{
			bool inc = false;

			char c = cur[i];

			if (c == ' ')
			{
				inc = true;
			}
			else if (c == '$')
			{
				int skip = 1;
				char j = 1;
				std::string str = "";
				while (j != '$') 
				{
					j = cur[i + skip];
					str.push_back(j);
					skip++;
				}

				str.pop_back();
				i += skip - 1;
				const char* strc = str.c_str();
				char* end;
				int ch = strtol(strc, &end, 10);
				if (end == strc)
				{
					// It was not a numeric, it was a color code
					if (str == "yellow")
					{
						TCODConsole::root->setDefaultForeground(TCODColor::yellow);
					}
					else if (str == "white")
					{
						TCODConsole::root->setDefaultForeground(TCODColor::white);
					}
					else if (str == "blue")
					{
						TCODConsole::root->setDefaultForeground(TCODColor(128, 128, 255));
					}
					else if (str == "sonar")
					{
						TCODConsole::root->setDefaultForeground(TCODColor(80, 255, 80));
					}
					else if (str == "brown")
					{
						TCODConsole::root->setDefaultForeground(TCODColor(158, 134, 100));
					}
					else if (str == "dark")
					{
						TCODConsole::root->setDefaultForeground(TCODColor(180, 180, 180));
					}
				}
				else
				{
					TCODConsole::root->putChar(x, y, ch);
					x++;
				}
			}
			else if(c == '@')
			{
				int skip = 1;
				char j = 1;
				std::string str = "";
				while (j != '@')
				{
					j = cur[i + skip];
					str.push_back(j);
					skip++;
				}

				str.pop_back();
				i+=str.size() + 1;

				bool hlight = false;
				if (mouse.cx >= x && mouse.cx < x + str.size() && mouse.cy == y)
				{
					TCODConsole::root->setDefaultBackground(TCODColor::white);
					TCODConsole::root->setDefaultForeground(TCODColor::black);
					hlight = true;
				}
				else
				{
					TCODConsole::root->setDefaultForeground(TCODColor(128, 128, 255));
				}

				for (int j = 0; j < str.size(); j++)
				{
					if (str[j] == '_')
					{
						str[j] = ' ';
					}

					TCODConsole::root->putChar(x + j, y, str[j]);
					if (hlight)
					{
						TCODConsole::root->setCharBackground(x + j, y, TCODColor::white);
					}

				}

				if (hlight && mouse.lbutton_pressed)
				{
					return_to.push_back(cur_screen);
					cur_screen = str;
				}

				TCODConsole::root->setDefaultBackground(TCODColor::black);

				x += str.size();

				TCODConsole::root->setDefaultForeground(TCODColor::white);

			}
			else if (c == '\n')
			{
				y++;
				x = 1;
			}
			else if (c != '$' && c > 32)
			{
				TCODConsole::root->putChar(x, y, cur[i]);
				inc = true;
			}

			if (inc)
			{
				x++;
				if (x >= 88)
				{
					y++;
					x = 1;
				}
			}
		}
		
	}
};
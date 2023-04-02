
	while(running)
	{
		SDL_WaitEvent(&event);
		switch(event.type)
		{
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_ESCAPE)
				{
					running = 0;
					break;
				}

				/* os_event_key(event.key.keysym.sym, 0); */
				break;

			case SDL_KEYUP:
				/* os_event_key(event.key.keysym.sym, 1); */
				break;

			case SDL_QUIT:
				running = 0;
				break;
		}


	}
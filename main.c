#include <stdio.h>
#include <vlc/vlc.h>
#include <vlc_common.h>
#include <errno.h>

#include <emscripten.h>
#include <emscripten/html5.h>

libvlc_media_player_t *mp;
libvlc_instance_t *libvlc; 
libvlc_time_t t = -1;

void iter()
{
	if (libvlc_media_player_get_time(mp) == t) {
		// when enable, the js does not respond.
		//libvlc_media_player_release( mp );
    	//libvlc_release( libvlc );
		emscripten_cancel_main_loop();
	}
	t = libvlc_media_player_get_time(mp);
}

int main() {

	/* We don't want to the main thread stop even if the main function exit.
	 * If this thread stop, all proxyfied functions wont be called.
	 */
	EM_ASM(Module['noExitRuntime']=true);
	emscripten_set_element_css_size("#canvas", 720, 540);
	//emscripten_set_canvas_element_size("#canvas", 720, 540);
	//emscripten_set_element_css_size("#canvas", 1080, 720);
    libvlc_media_t *m;

    char const *vlc_argv[] = {
	    "-vvv",
		"--no-spu",
		"--no-osd",
        "-Idummy",
		"--aout", "openal",
		"--vout", "gles2",
		"--ignore-config",
	};

	libvlc = libvlc_new( ARRAY_SIZE( vlc_argv ), vlc_argv );

    if (libvlc == NULL)
    {
        fprintf( stderr, "unable to create libvlc instance" );
        return -1;
    }

	m = libvlc_media_new_path( libvlc, "Dolby_Canyon.vob" );

	if (m == NULL)
	{
		fprintf(stderr, "unable to create media");
		return -1;
	}
	mp = libvlc_media_player_new_from_media( m );
	if (mp == NULL)
	{
		fprintf(stderr, "unable to create media player");
		return -1;
	}

	libvlc_media_release( m );
	m = libvlc_media_player_get_media(mp);
	libvlc_audio_set_volume (mp, 100);
	int res = libvlc_media_player_play (mp);
	if (res != 0) {	
        fprintf( stderr, "unable to play media" );
		return -1;
	}
	
	emscripten_set_main_loop(iter, 1, 1);

    return 0;
}

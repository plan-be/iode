#include "scr.h"


/*NH*/
SCR_move_image(im1, im2)
IMAGE   **im1;
IMAGE   *im2;
{
    int     c;
    IMAGE   *s_im;
    STATUS  *sta;

    sta = SCR_save_status();
    SCR_cursor_off();

    /* ReConstruct background */
    s_im = SCR_save_image(0, 0, SCR_PAGE_SIZE[0], SCR_PAGE_SIZE[1]);

    while(1) {
	c = SCR_get_key();
	switch(c) {
	    case SCR_CSR_HOME:
		im2->im_line = 0;
		im2->im_column = 0;
		SCR_mv_im(s_im, im1, im2);
		break;
	    case SCR_CSR_END:
		im2->im_line   = SCR_PAGE_SIZE[0] - im2->im_nb_line;
		im2->im_column = SCR_PAGE_SIZE[1] - im2->im_nb_column;
		SCR_mv_im(s_im, im1, im2);
		break;
	    case SCR_TAB:
		if(im2->im_column + 4 > SCR_PAGE_SIZE[1] - im2->im_nb_column)
		    im2->im_column = SCR_PAGE_SIZE[1] - im2->im_nb_column;
		else im2->im_column += 4;
		SCR_mv_im(s_im, im1, im2);
		break;
	    case SCR_S_TAB:
		if(im2->im_column < 4) im2->im_column = 0;
		else im2->im_column -= 4;
		SCR_mv_im(s_im, im1, im2);
		break;
	    case SCR_CSR_UP:
		if(im2->im_line > 0) im2->im_line --;
		SCR_mv_im(s_im, im1, im2);
		break;
	    case SCR_CSR_DOWN:
		im2->im_line = _min_(SCR_PAGE_SIZE[0] - im2->im_nb_line,
					im2->im_line + 1);
		SCR_mv_im(s_im, im1, im2);
		break;
	    case SCR_CSR_LEFT:
		if(im2->im_column > 0) im2->im_column--;
		SCR_mv_im(s_im, im1, im2);
		break;
	    case SCR_CSR_RIGHT:
		im2->im_column = _min_(SCR_PAGE_SIZE[1] - im2->im_nb_column,
					im2->im_column + 1);
		SCR_mv_im(s_im, im1, im2);
		break;
	    case SCR_MOVE_KEY:
	    case SCR_ENTER:
	    case SCR_ESCAPE:
		SCR_free_image(s_im);
		SCR_reset_status(sta);
		return(0); /* JMP 17-01-98 */
	    default:
		SCR_beep();
		break;
	}
    }
}

/*NH*/
SCR_mv_im(s_im, im1, im2)
IMAGE   *s_im, **im1, *im2;
{
    /* RESET FULL SCREEN */
    SCR_im_to_im(s_im, *im1, 0, 0);
    /* SAVE BACK */
    SCR_free_image(*im1);
    *im1 = SCR_extract_image(s_im, im2->im_line, im2->im_column,
			  im2->im_nb_line, im2->im_nb_column);
    /* SAVE IMAGE MOVED */
    SCR_im_to_im(s_im, im2, 0, 0);
    /* DISPLAY */
    SCR_display_image(s_im);
    return(0);
}


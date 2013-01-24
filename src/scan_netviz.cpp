/**
 * pcap visualization engine 
 */

#include "config.h"
#include <iostream>
#include <sys/types.h>

#include "bulk_extractor_i.h"

#ifdef CAIRO_PDF_AVAILABLE
#include "netviz/one_page_report.h"
#include "netviz/time_histogram.h"
one_page_report *th_one_page=0;
#endif


#ifdef CAIRO_PDF_AVAILABLE
static void th_startup()
{
    if(th_one_page==0) th_one_page = new one_page_report();
}

static void th_process_packet(void *user,const packet_info &pi)
{
    th_one_page->ingest_packet(pi);
}

static void th_shutdown(const class scanner_params &sp)
{
    th_one_page->source_identifier = sp.fs.input_fname;
    th_one_page->render(sp.fs.outdir);
    delete th_one_page;
    th_one_page = 0;
}
#endif


extern "C"
void  scan_netviz(const class scanner_params &sp,const recursion_control_block &rcb)
{
    if(sp.sp_version!=scanner_params::CURRENT_SP_VERSION){
	std::cerr << "scan_timehistogram requires sp version "
		  << scanner_params::CURRENT_SP_VERSION << "; "
		  << "got version " << sp.sp_version << "\n";
	exit(1);
    }

    if(sp.phase==scanner_params::startup){
	sp.info->name  = "netviz";
	sp.info->flags = scanner_info::SCANNER_DISABLED;
	sp.info->author= "Mike Shick";
	sp.info->packet_user = 0;
#ifdef CAIRO_PDF_AVAILABLE
	sp.info->packet_cb = th_process_packet;
	th_startup();
#else
	sp.info->packet_cb = 0;
#endif	
    }

    if(sp.phase==scanner_params::scan){	// this is for scanning sbufs
	return;
    }

    if(sp.phase==scanner_params::shutdown){
#ifdef CAIRO_PDF_AVAILABLE
	th_shutdown(sp);
#endif
    }
}

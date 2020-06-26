# pragma once

# include <sig/sn_poly_editor.h>
# include <sig/sn_lines2.h>

# include <sigogl/ui_button.h>
# include <sigogl/ws_viewer.h>

#include "sn_mynode.h"

//new one:
// Viewer for this example:
class MyViewer : public WsViewer
{
private:
	SnTransform* _t1;
protected:
	enum MenuEv { EvNormals, EvAnimate, EvExit };
	UiCheckButton* _nbut;
	UiCheckButton* mcolorbut;
	bool _animating;
public:
	MyViewer(int x, int y, int w, int h, const char* l);
	void build_ui();
	void add_model(SnShape* s, GsVec p);
	void add_mynode(int n);
	void build_scene();
	void show_normals(bool view);
	void run_animation();
	virtual int handle_keyboard(const GsEvent& e) override;
	virtual int uievent(int e) override;

};

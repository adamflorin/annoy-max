/**
* annoy.cpp: wrapper around the annoy Approximate Nearest Neighbor search
* https://github.com/spotify/annoy
*
* Copyright 2018 Adam Florin
*/

#include "ext.h"
#include "ext_obex.h"

#include "annoylib.h"
#include "kissrandom.h"

extern "C" {
	// External struct
	typedef struct _annoy {
		t_object object;
		AnnoyIndex<int, float, Angular, Kiss32Random> *index;
	} t_annoy;

	// Method prototypes
	void *annoy_new(t_symbol *s, long argc, t_atom *argv);
	void annoy_free(t_annoy *x);
	void annoy_assist(t_annoy *x, void *b, long m, long a, char *s);
	void annoy_load(t_annoy *x, t_symbol *s);
	void annoy_unload(t_annoy *x);

	// External class
	static t_class *annoy_class = NULL;

	/** Initialize external class */
	void ext_main(void *r) {
		t_class *c = class_new(
			"annoy",
			(method)annoy_new,
			(method)annoy_free,
			(long)sizeof(t_annoy),
			0L,
			A_GIMME,
			0
		);

		class_addmethod(c, (method)annoy_assist, "assist", A_CANT, 0);
		class_addmethod(c, (method)annoy_load, "load", A_SYM, 0);
		class_addmethod(c, (method)annoy_unload, "unload", 0);

		class_register(CLASS_BOX, c);
		annoy_class = c;
	}

	/** Initialize external instance */
	void *annoy_new(t_symbol *s, long argc, t_atom *argv) {
		t_annoy *x = (t_annoy *)object_alloc(annoy_class);

		if (!x) {
			return x;
		}

		// annoy
		int f = 20;
		x->index = new AnnoyIndex<int, float, Angular, Kiss32Random>(f);

		return x;
	}

	/** Destroy external instance */
	void annoy_free(t_annoy *x) {
		annoy_unload(x);
	}

	/** Configure user tooltip prompts */
	void annoy_assist(t_annoy *x, void *b, long m, long a, char *s) {
		if (m == ASSIST_INLET) {
			sprintf(s, "I am inlet %ld", a);
		} else {
			sprintf(s, "I am outlet %ld", a);
		}
	}

	void annoy_load(t_annoy *x, t_symbol *s) {
		bool loaded = x->index->load(s->s_name);
		if (loaded) {
			object_post((t_object*)x, "Loaded annoy file '%s'", s->s_name);
		} else {
			object_error((t_object*)x, "Failed to load annoy file '%s'", s->s_name);
		}
	}

	void annoy_unload(t_annoy *x) {
		x->index->unload();
		object_post((t_object*)x, "Unloaded annoy file");
	}
}

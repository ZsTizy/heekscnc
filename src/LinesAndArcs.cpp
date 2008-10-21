// LinesAndArcs.cpp

#include "stdafx.h"
#include "LinesAndArcs.h"
#include "geometry/geometry.h"
#include "../../HeeksCAD/interface/HeeksObj.h"

void add_to_kurve(HeeksObj* object, Kurve& kurve)
{

	if(object->GetType() == LineType)
	{
		double s[3], e[3];
		if(!object->GetStartPoint(s))return;
		if(!object->GetEndPoint(e))return;
		kurve.Add(Span(0, Point(s[0], s[1]), Point(e[0], e[1]), Point(0, 0)));
	}
	else if(object->GetType() == ArcType)
	{
		double s[3], e[3];
		double c[3], a[3];
		if(!object->GetStartPoint(s))return;
		if(!object->GetEndPoint(e))return;
		if(!heeksCAD->GetArcCentre(object, c))return;
		if(!heeksCAD->GetArcAxis(object, a))return;
		int type = (a[2] >= 0) ? 1 : -1;
		kurve.Add(Span(type, Point(s[0], s[1]), Point(e[0], e[1]), Point(c[0], c[1])));
	}
}

void add_to_kurve(std::list<HeeksObj*> &list, Kurve &kurve)
{
	for(std::list<HeeksObj*>::iterator It = list.begin(); It != list.end(); It++)
	{
		HeeksObj* object = *It;
		add_to_kurve(object, kurve);
	}
}

void create_lines_and_arcs(Kurve &kurve, bool undoably)
{
	std::vector<Span> spans;
	kurve.StoreAllSpans(spans);

	double up[3] = {0, 0, 1};
	double down[3] = {0, 0, -1};

	for(unsigned int i = 0; i<spans.size(); i++)
	{
		Span& span = spans[i];
		if(span.dir)
		{
			double s[3] = {span.p0.x, span.p0.y, 0};
			double e[3] = {span.p1.x, span.p1.y, 0};
			double c[3] = {span.pc.x, span.pc.y, 0};
			if(fabs(s[0] - e[0]) > heeksCAD->GetTolerance() || fabs(s[1] - e[1]) > heeksCAD->GetTolerance())
			{
				HeeksObj* new_object = heeksCAD->NewArc(s, e, c, span.dir > 0 ? up : down);
				if(undoably)heeksCAD->AddUndoably(new_object, NULL);
				else heeksCAD->GetMainObject()->Add(new_object, NULL);
			}
		}
		else
		{
			double s[3] = {span.p0.x, span.p0.y, 0};
			double e[3] = {span.p1.x, span.p1.y, 0};
			if(fabs(s[0] - e[0]) > heeksCAD->GetTolerance() || fabs(s[1] - e[1]) > heeksCAD->GetTolerance())
			{
				HeeksObj* new_object = heeksCAD->NewLine(s, e);
				if(undoably)heeksCAD->AddUndoably(new_object, NULL);
				else heeksCAD->GetMainObject()->Add(new_object, NULL);
			}
		}
	}
}
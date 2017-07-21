/*******************************************************************************************************
*  Copyright 2017 Alliance for Sustainable Energy, LLC
*
*  NOTICE: This software was developed at least in part by Alliance for Sustainable Energy, LLC
*  (�Alliance�) under Contract No. DE-AC36-08GO28308 with the U.S. Department of Energy and the U.S.
*  The Government retains for itself and others acting on its behalf a nonexclusive, paid-up,
*  irrevocable worldwide license in the software to reproduce, prepare derivative works, distribute
*  copies to the public, perform publicly and display publicly, and to permit others to do so.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted
*  provided that the following conditions are met:
*
*  1. Redistributions of source code must retain the above copyright notice, the above government
*  rights notice, this list of conditions and the following disclaimer.
*
*  2. Redistributions in binary form must reproduce the above copyright notice, the above government
*  rights notice, this list of conditions and the following disclaimer in the documentation and/or
*  other materials provided with the distribution.
*
*  3. The entire corresponding source code of any redistribution, with or without modification, by a
*  research entity, including but not limited to any contracting manager/operator of a United States
*  National Laboratory, any institution of higher learning, and any non-profit organization, must be
*  made publicly available under this license for as long as the redistribution is made available by
*  the research entity.
*
*  4. Redistribution of this software, without modification, must refer to the software by the same
*  designation. Redistribution of a modified version of this software (i) may not refer to the modified
*  version by the same designation, or by any confusingly similar designation, and (ii) must refer to
*  the underlying software originally provided by Alliance as �System Advisor Model� or �SAM�. Except
*  to comply with the foregoing, the terms �System Advisor Model�, �SAM�, or any confusingly similar
*  designation may not be used to refer to any modified version of this software or any modified
*  version of the underlying software originally provided by Alliance without the prior written consent
*  of Alliance.
*
*  5. The name of the copyright holder, contributors, the United States Government, the United States
*  Department of Energy, or any of their employees may not be used to endorse or promote products
*  derived from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
*  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
*  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER,
*  CONTRIBUTORS, UNITED STATES GOVERNMENT OR UNITED STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR
*  EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
*  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************************************/

#include <cmath>
#include <numeric>

#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/srchctrl.h>

#include <wex/plot/plaxis.h>
#include <wex/plot/plbarplot.h>
#include <wex/plot/pllineplot.h>
#include <wex/plot/plscatterplot.h>

#include <wex/dview/dvselectionlist.h>

#include <wex/radiochoice.h>
#include <wex/utils.h>
#include <wex/exttext.h>
#include <wex/metro.h>
#include <wex/snaplay.h>

#include "case.h"
#include "graph.h"
#include "variables.h"
#include "simulation.h"
#include "results.h"

Graph::Graph()
{
	Type = BAR;
	ShowXValues = ShowYValues = ShowLegend = true;
	LegendPos = wxPLPlotCtrl::RIGHT;
	Size=0;
	CoarseGrid = FineGrid = true;	
	YMin=YMax= std::numeric_limits<double>::quiet_NaN();
	FontScale = 1;
	FontFace = 1;  // default to 'modern'
}

void Graph::Copy(Graph *gr)
{
	Type = gr->Type;
	Y = gr->Y;
	
	XLabel = gr->XLabel;
	YLabel = gr->YLabel;
	Title = gr->Title;
	ShowXValues = gr->ShowXValues;
	ShowYValues = gr->ShowYValues;
	ShowLegend = gr->ShowLegend;
	LegendPos = gr->LegendPos;
	Size = gr->Size;
	CoarseGrid = gr->CoarseGrid;
	FineGrid = gr->FineGrid;
	YMin = gr->YMin;
	YMax = gr->YMax;
	Notes = gr->Notes;
	FontScale = gr->FontScale;
	FontFace = gr->FontFace;
}


bool Graph::SameAs(Graph *gr)
{
	return ( this->Title == gr->Title && this->Y == gr->Y );
}

bool Graph::Write( wxOutputStream &os )
{
	wxDataOutputStream ds(os);
	ds.Write16( 0xfd ); // identifier
	ds.Write8( 1 ); // version

	ds.Write32( Type );
	ds.WriteString( Title );
	ds.Write32( Y.Count() );
	for (int i=0;i<(int)Y.Count();i++)
		ds.WriteString( Y[i] );

	ds.WriteString( XLabel );
	ds.WriteString( YLabel );
	ds.Write8( ShowXValues ? 1 : 0 );
	ds.Write8( ShowYValues ? 1 : 0  );
	ds.Write8( ShowLegend ? 1 : 0  );
	ds.Write8( LegendPos );
	ds.Write8( Size );
	ds.Write8( CoarseGrid ? 1 : 0  );
	ds.Write8( FineGrid ? 1 : 0  );
	ds.WriteDouble( YMin );
	ds.WriteDouble( YMax );
	ds.WriteString( Notes );
	ds.WriteDouble( FontScale );
	ds.Write8( FontFace );

	ds.Write16( 0xfd ); // identifier
	return true;
}


bool Graph::Read( wxInputStream &is )
{
	size_t i, count;
	wxDataInputStream ds(is);

	unsigned short identifier = ds.Read16();
	/*unsigned char ver = */ ds.Read8(); // read the version number, not currently used...

	Type = ds.Read32();
	Title = ds.ReadString();

	Y.Clear();
	count = ds.Read32();
	for (i=0;i<count;i++)
		Y.Add( ds.ReadString() );

	XLabel = ds.ReadString();
	YLabel = ds.ReadString();
	
	ShowXValues = ds.Read8() ? true : false;
	ShowYValues = ds.Read8() ? true : false;
	ShowLegend = ds.Read8() ? true : false;
	LegendPos = ds.Read8();

	Size = ds.Read8();
	CoarseGrid = ds.Read8() ? true : false;
	FineGrid = ds.Read8() ? true : false;
	
	YMin = ds.ReadDouble();
	YMax = ds.ReadDouble();
	
	Notes = ds.ReadString();
	FontScale = ds.ReadDouble();
	FontFace = ds.Read8();

	return identifier == ds.Read16();
}


DEFINE_EVENT_TYPE( wxEVT_GRAPH_SELECT )

BEGIN_EVENT_TABLE( GraphCtrl, wxPLPlotCtrl )
	EVT_LEFT_DOWN( GraphCtrl::OnLeftDown )
END_EVENT_TABLE()


GraphCtrl::GraphCtrl( wxWindow *parent, int id )
	: wxPLPlotCtrl( parent, id, wxDefaultPosition, wxScaleSize(500,400) )
{
	SetBackgroundColour( *wxWHITE );
}


std::vector<wxColour> &Graph::Colours()
{
	
static std::vector<wxColour> s_colours;
	if ( s_colours.size() == 0 )
	{
		s_colours.push_back( wxColour(111,164,196) );
		s_colours.push_back( wxColour("GREY") );
		s_colours.push_back( wxColour(181,211,227) );
		s_colours.push_back( *wxLIGHT_GREY );
		s_colours.push_back( wxColour("PALE GREEN") );
		s_colours.push_back( wxColour("GOLDENROD") );
		s_colours.push_back( wxColour("MEDIUM VIOLET RED") );
		s_colours.push_back( wxColour("MEDIUM SEA GREEN") );
		s_colours.push_back( wxColour("DARK SLATE GREY") );
		s_colours.push_back( wxColour("WHEAT") );
		s_colours.push_back( wxColour("FIREBRICK") );
		s_colours.push_back( wxColour("dark orchid") );
		s_colours.push_back( wxColour("dim grey") );
		s_colours.push_back( wxColour("brown") );
	}

	return s_colours;
}

void GraphCtrl::Display( Simulation *sim, Graph &gi )
{
static const char *s_monthNames[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
										"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

	std::vector<wxColour> &s_colours = Graph::Colours();

	m_s = sim;
	m_g.Copy( &gi );

	DeleteAllPlots();
	
	if ( !m_s )
	{
		Refresh();
		return;
	}
	
	// setup visual properties of graph
	wxFont font( *wxNORMAL_FONT );
	switch( m_g.FontFace )
	{
	case 1: font = wxMetroTheme::Font( wxMT_LIGHT ); break;
	case 2: font = *wxSWISS_FONT; break;
	case 3: font = wxFont( 12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL ); break;
	case 4: font = wxFont( 12, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL ); break;
	}
	
	if ( m_g.FontScale != 0.0 )
	{
		int points = (int)(((double)font.GetPointSize())*m_g.FontScale);
		if ( points < 4 ) points = 4;
		if ( points > 32 ) points = 32;
		font.SetPointSize( points );		
	}
	
	SetFont( font );

	ShowGrid( m_g.CoarseGrid, m_g.FineGrid );
	SetTitle( m_g.Title );
	ShowLegend( m_g.ShowLegend );	
	SetLegendLocation( (wxPLPlotCtrl::LegendPos)m_g.LegendPos );
	
	// setup data
	std::vector<VarValue*> yvars;
	wxArrayString ynames;
	int ndata = -1;

	for( size_t i=0;i<m_g.Y.size();i++ )
	{
		if ( VarValue *vv = m_s->GetValue( m_g.Y[i] ) )
		{
			int count = 0;
			if ( vv->Type() == VV_NUMBER )
				count = 1;
			else if ( vv->Type() == VV_ARRAY )
				count = vv->Length();

			if ( i == 0 ) ndata = count;
			else if ( ndata != count ) ndata = -1;

			if ( count > 0 )
			{
				yvars.push_back( vv );
				ynames.push_back( m_g.Y[i] );
			}
		}
	}

	if ( ndata < 0 )
	{
		SetTitle( "All variables must have the same number of data values." );
		Refresh();
		return;
	}

	std::vector< std::vector<wxRealPoint> > plotdata( yvars.size() );

	int cidx = 0; // colour index
	wxPLBarPlot *last_bar = 0;
	std::vector<wxPLBarPlot*> bar_group;

	for( size_t i=0;i<yvars.size();i++ )
	{
		if ( yvars[i]->Type() == VV_ARRAY )
		{
			size_t n = 0;
			float *p = yvars[i]->Array( &n );

			plotdata[i].reserve( ndata );
			for( size_t k=0;k<n;k++ )
			{
				if (std::isnan(p[k]))
					plotdata[i].push_back(wxRealPoint(k, 0));
				else
					plotdata[i].push_back(wxRealPoint(k, p[k]));
			}
		}
		else
		{
			if (std::isnan(yvars[i]->Value()))
				plotdata[i].push_back(wxRealPoint(i, 0));
			else
				plotdata[i].push_back(wxRealPoint(i, yvars[i]->Value()));
		}

		wxPLPlottable *plot = 0;
		if ( m_g.Type == Graph::LINE )
			plot = new wxPLLinePlot( plotdata[i], m_s->GetLabel( ynames[i] ), s_colours[cidx], 
				wxPLLinePlot::SOLID, m_g.Size+2 );
		else if ( m_g.Type == Graph::BAR || m_g.Type == Graph::STACKED )
		{
			wxPLBarPlot *bar = new wxPLBarPlot(  plotdata[i], 0.0, m_s->GetLabel(ynames[i]), s_colours[cidx] );
			if ( m_g.Size != 0 )
				bar->SetThickness( m_g.Size );

			if ( m_g.Type == Graph::STACKED )
				bar->SetStackedOn( last_bar );
			else
				bar_group.push_back( bar );

			last_bar = bar;
			plot = bar;
		}
		else if ( m_g.Type == Graph::SCATTER )
		{
			plot = new wxPLScatterPlot( plotdata[i], m_s->GetLabel( ynames[i] ), s_colours[cidx], m_g.Size+2 );
			if ( plotdata[i].size() < 100 )
				plot->SetAntiAliasing( true );
		}


		if ( ++cidx >= s_colours.size() ) cidx = 0; // incr and wrap around colour index
		
		if ( plot != 0 )
			AddPlot( plot, wxPLPlotCtrl::X_BOTTOM, wxPLPlotCtrl::Y_LEFT, wxPLPlotCtrl::PLOT_TOP, false );
	}

	
	// group the bars together if they're not stacked and not single values
	if ( ndata > 1 && m_g.Type == Graph::BAR )
		for( size_t i=0;i<bar_group.size();i++ )
			bar_group[i]->SetGroup( bar_group );

	// create the axes

	// x-axis
	if (ndata == 1)
	{
		// single value axis
		wxPLLabelAxis *x1 = new wxPLLabelAxis(-1, yvars.size(), m_g.XLabel);
		if (m_g.ShowXValues)
		{
			for (size_t i = 0; i < ynames.size(); i++)
				x1->Add(i, m_s->GetLabel(ynames[i]));
		}
		SetXAxis1(x1);
	}
	else if (ndata == 12)
	{
		// month axis
		wxPLLabelAxis *x1 = new wxPLLabelAxis(-1, 12, m_g.XLabel);
		for (size_t i = 0; i < 12; i++)
			x1->Add(i, s_monthNames[i]);
		SetXAxis1(x1);
	}
	else
	{
		// linear axis
		SetXAxis1(new wxPLLinearAxis(-1, ndata + 1, m_g.XLabel));
	}
	// setup y axis

	if ( GetPlotCount() > 0 )
	{
		double ymin, ymax;
		GetPlot(0)->GetMinMax( 0, 0, &ymin, &ymax );
		for( size_t i=1;i<GetPlotCount();i++ )
			GetPlot(i)->ExtendMinMax( 0, 0, &ymin, &ymax );

		if ( m_g.Type == Graph::STACKED || m_g.Type == Graph::BAR )
		{ // forcibly include the zero line for bar plots
			if ( ymin > 0 ) ymin = 0;
			if ( ymax < 0 ) ymax = 0;
		}
		
		double yadj = (ymax-ymin)*0.05;
		
		if (ymin != 0) ymin -= yadj;
		if (ymax != 0) ymax += yadj;

		if (ymin == ymax) {
			// no variation in y values, so pick some reasonable graph bounds
			ymax = (ymax == 0) ? 1 : ymax += ymax*0.05;
			ymin = (ymin == 0) ? -1 : ymin -= ymin*0.05;
		}

		SetYAxis1( new wxPLLinearAxis( ymin, ymax, m_g.YLabel ) );
	}

	Invalidate();
	Refresh();
}

void GraphCtrl::Display(std::vector<Simulation *>sims, Graph &gi)
{
	static const char *s_monthNames[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

	static std::vector<wxColour> s_colours;
	if (s_colours.size() == 0)
	{
		s_colours.push_back(wxColour(111, 164, 196));
		s_colours.push_back(wxColour("GREY"));
		s_colours.push_back(wxColour(181, 211, 227));
		s_colours.push_back(*wxLIGHT_GREY);
		s_colours.push_back(wxColour("PALE GREEN"));
		s_colours.push_back(wxColour("GOLDENROD"));
		s_colours.push_back(wxColour("MEDIUM VIOLET RED"));
		s_colours.push_back(wxColour("MEDIUM SEA GREEN"));
		s_colours.push_back(wxColour("DARK SLATE GREY"));
		s_colours.push_back(wxColour("WHEAT"));
		s_colours.push_back(wxColour("FIREBRICK"));
		s_colours.push_back(wxColour("dark orchid"));
		s_colours.push_back(wxColour("dim grey"));
		s_colours.push_back(wxColour("brown"));
	}

	m_g.Copy(&gi);

	DeleteAllPlots();

	if ((sims.size() <= 0) || (m_g.Y.Count() > 1))
	{
		Refresh();
		return;
	}
	m_s = sims[0];

	// setup visual properties of graph
	wxFont font(*wxNORMAL_FONT);
	switch (m_g.FontFace)
	{
	case 1: font = wxMetroTheme::Font(wxMT_LIGHT); break;
	case 2: font = *wxSWISS_FONT; break;
	case 3: font = wxFont(12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL); break;
	case 4: font = wxFont(12, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL); break;
	}

	if (m_g.FontScale != 0.0)
	{
		int points = (int)(((double)font.GetPointSize())*m_g.FontScale);
		if (points < 4) points = 4;
		if (points > 32) points = 32;
		font.SetPointSize(points);
	}

	SetFont(font);

	ShowGrid(m_g.CoarseGrid, m_g.FineGrid);
	SetTitle(m_g.Title);
	ShowLegend(m_g.ShowLegend);
	SetLegendLocation((wxPLPlotCtrl::LegendPos)m_g.LegendPos);

	// setup data
	std::vector<VarValue*> yvars;
	wxArrayString ynames;
	int ndata = -1;

	for (size_t i = 0; i<sims.size(); i++)
	{
		if (VarValue *vv = sims[i]->GetValue(m_g.Y[0]))
		{
			int count = 0;
			if (vv->Type() == VV_NUMBER)
				count = 1;
			else if (vv->Type() == VV_ARRAY)
				count = vv->Length();

			if (i == 0) ndata = count;
			else if (ndata != count) ndata = -1;

			if (count > 0)
			{
				yvars.push_back(vv);
				ynames.push_back(sims[i]->GetLabel(m_g.Y[0]) + wxString::Format(" : run %d", (int)(i+1)));
			}
		}
	}

	if (ndata < 0)
	{
		SetTitle("All variables must have the same number of data values.");
		Refresh();
		return;
	}

	std::vector< std::vector<wxRealPoint> > plotdata(yvars.size());

	int cidx = 0; // colour index
	wxPLBarPlot *last_bar = 0;
	std::vector<wxPLBarPlot*> bar_group;

	for (size_t i = 0; i<yvars.size(); i++)
	{
		if (yvars[i]->Type() == VV_ARRAY)
		{
			size_t n = 0;
			float *p = yvars[i]->Array(&n);

			plotdata[i].reserve(ndata);
			for (size_t k = 0; k < n; k++)
			{
				if (std::isnan(p[k]))
					plotdata[i].push_back(wxRealPoint(k, 0));
				else
					plotdata[i].push_back(wxRealPoint(k, p[k]));
			}
		}
		else
		{
			if (std::isnan(yvars[i]->Value()))
				plotdata[i].push_back(wxRealPoint(i, 0));
			else
				plotdata[i].push_back(wxRealPoint(i, yvars[i]->Value()));
		}

		wxPLPlottable *plot = 0;
		if (m_g.Type == Graph::LINE)
			plot = new wxPLLinePlot(plotdata[i], ynames[i], s_colours[cidx],
			wxPLLinePlot::SOLID, m_g.Size + 2);
		else if (m_g.Type == Graph::BAR || m_g.Type == Graph::STACKED)
		{
			wxPLBarPlot *bar = new wxPLBarPlot(plotdata[i], 0.0, ynames[i], s_colours[cidx]);
			if (m_g.Size != 0)
				bar->SetThickness(m_g.Size);

			if (m_g.Type == Graph::STACKED)
				bar->SetStackedOn(last_bar);
			else
				bar_group.push_back(bar);

			last_bar = bar;
			plot = bar;
		}
		else if (m_g.Type == Graph::SCATTER)
		{
			plot = new wxPLScatterPlot(plotdata[i], ynames[i], s_colours[cidx], m_g.Size + 2);
			if (plotdata[i].size() < 100)
				plot->SetAntiAliasing(true);
		}


		if (++cidx >= s_colours.size()) cidx = 0; // incr and wrap around colour index

		if (plot != 0)
			AddPlot(plot, wxPLPlotCtrl::X_BOTTOM, wxPLPlotCtrl::Y_LEFT, wxPLPlotCtrl::PLOT_TOP, false);
	}


	// group the bars together if they're not stacked and not single values
	if (ndata > 1 && m_g.Type == Graph::BAR)
		for (size_t i = 0; i<bar_group.size(); i++)
			bar_group[i]->SetGroup(bar_group);

	// create the axes
	if (ndata == 1)
	{
		// single value axis
		wxPLLabelAxis *x1 = new wxPLLabelAxis(-1, yvars.size(), m_g.XLabel);
		for (size_t i = 0; i<ynames.size(); i++)
			x1->Add(i, wxString::Format("%d", (int)( i+1)));
//			x1->Add(i, ynames[i]);
		SetXAxis1(x1);
	}
	else if (ndata == 12)
	{
		// month axis
		wxPLLabelAxis *x1 = new wxPLLabelAxis(-1, 12, m_g.XLabel);
		for (size_t i = 0; i<12; i++)
			x1->Add(i, s_monthNames[i]);
		SetXAxis1(x1);
	}
	else
	{
		// linear axis
		SetXAxis1(new wxPLLinearAxis(-1, ndata + 1, m_g.XLabel));
	}


	// setup y axis

	if (GetPlotCount() > 0)
	{
		double ymin, ymax;
		GetPlot(0)->GetMinMax(0, 0, &ymin, &ymax);
		for (size_t i = 1; i<GetPlotCount(); i++)
			GetPlot(i)->ExtendMinMax(0, 0, &ymin, &ymax);

		if (m_g.Type == Graph::STACKED || m_g.Type == Graph::BAR)
		{ // forcibly include the zero line for bar plots
			if (ymin > 0) ymin = 0;
			if (ymax < 0) ymax = 0;
		}

		double yadj = (ymax - ymin)*0.05;

		if (ymin != 0) ymin -= yadj;
		if (ymax != 0) ymax += yadj;

		if (ymin == ymax) {
			// no variation in y values, so pick some reasonable graph bounds
			ymax = (ymax == 0) ? 1 : ymax += ymax*0.05;
			ymin = (ymin == 0) ? -1 : ymin -= ymin*0.05;
		}

		SetYAxis1(new wxPLLinearAxis(ymin, ymax, m_g.YLabel));
	}


	Invalidate();
	Refresh();
}



void GraphCtrl::OnLeftDown( wxMouseEvent &evt )
{
	wxCommandEvent e( wxEVT_GRAPH_SELECT, GetId() );
	e.SetEventObject( this );
	GetEventHandler()->ProcessEvent( e );
	
	evt.Skip();
}



DEFINE_EVENT_TYPE( wxEVT_GRAPH_PROPERTY_CHANGE )

enum { ID_Y = wxID_HIGHEST+495, ID_TYPE, ID_TITLE, ID_XLABEL, ID_YLABEL, ID_SHOW_LEGEND, ID_LEGENDPOS, 
	ID_SCALE, ID_SIZE, ID_COARSE, ID_FINE, ID_FONT_FACE, ID_SRCH };

BEGIN_EVENT_TABLE( GraphProperties, wxPanel )
	EVT_DVSELECTIONLIST( ID_Y, GraphProperties::OnEdit )
	EVT_TEXT( ID_TITLE, GraphProperties::OnEdit )
	EVT_TEXT( ID_XLABEL, GraphProperties::OnEdit )
	EVT_TEXT(ID_YLABEL, GraphProperties::OnEdit)
	EVT_TEXT(ID_SRCH, GraphProperties::OnSearch)
	EVT_RADIOBUTTON(ID_TYPE, GraphProperties::OnEdit)
	EVT_COMMAND_SCROLL( ID_SCALE, GraphProperties::OnSlider )
	EVT_COMMAND_SCROLL( ID_SIZE, GraphProperties::OnSlider )
	EVT_CHECKBOX( ID_COARSE, GraphProperties::OnEdit )
	EVT_CHECKBOX( ID_FINE, GraphProperties::OnEdit )
	EVT_CHECKBOX( ID_SHOW_LEGEND, GraphProperties::OnEdit )
	EVT_CHOICE( ID_LEGENDPOS, GraphProperties::OnEdit )
	EVT_CHOICE( ID_FONT_FACE, GraphProperties::OnEdit )
END_EVENT_TABLE()


GraphProperties::GraphProperties( wxWindow *parent, int id )
	: wxPanel( parent, id )
{
	m_srch = new wxSearchCtrl(this, ID_SRCH);

	m_Y = new wxDVSelectionListCtrl( this, ID_Y, 1, wxDefaultPosition, wxDefaultSize, wxDVSEL_NO_COLOURS );
	m_Y->SetBackgroundColour( *wxWHITE );

	m_type = new wxRadioChoice( this, ID_TYPE );
	m_type->SetHorizontal( true );
	m_type->Add( "Bar" );
	m_type->Add( "Stack" );
	m_type->Add( "Line" );
	m_type->Add( "Scatter" );
	m_type->SetSelection( 0 );

	m_title = new wxExtTextCtrl( this, ID_TITLE );
	m_xlabel = new wxExtTextCtrl( this, ID_XLABEL );
	m_ylabel = new wxExtTextCtrl( this, ID_YLABEL );
	
	m_size = new wxSlider( this, ID_SIZE, 0, 0, 35);
	m_scale = new wxSlider( this, ID_SCALE, 10, 5, 15 );

	m_coarse = new wxCheckBox( this, ID_COARSE, "Coarse grid" );
	m_coarse->SetValue( true );
	m_fine = new wxCheckBox( this, ID_FINE, "Fine grid" );
	m_fine->SetValue( true );


	m_showLegend = new wxCheckBox( this, ID_SHOW_LEGEND, "Legend" );

	wxString lpos[] = { "Manual", 
		"Northwest", "Southwest", "Northeast", "Southeast", 
		"North", "South", "East", "West", 
		"Bottom", "Right" };
	m_legendPos = new wxChoice( this, ID_LEGENDPOS, wxDefaultPosition, wxDefaultSize, 11, lpos );

	wxString faces[] = { "Default", "Modern", "Sanserif", "Serif", "Fixed" };
	m_font = new wxChoice( this, ID_FONT_FACE, wxDefaultPosition, wxDefaultSize, 5, faces );
	
	wxFlexGridSizer *prop_sizer = new wxFlexGridSizer( 2 );
	prop_sizer->AddGrowableCol( 1 );

	prop_sizer->Add( new wxStaticText( this, wxID_ANY, "Title:" ), 0, wxALL|wxALIGN_CENTER_VERTICAL, 2 );
	prop_sizer->Add( m_title, 0, wxALL|wxEXPAND, 1 );
	
	prop_sizer->Add( new wxStaticText( this, wxID_ANY, "X label:" ), 0, wxALL|wxALIGN_CENTER_VERTICAL, 2 );
	prop_sizer->Add( m_xlabel, 0, wxALL|wxEXPAND, 1 );
	
	prop_sizer->Add( new wxStaticText( this, wxID_ANY, "Y label:" ), 0, wxALL|wxALIGN_CENTER_VERTICAL, 2 );
	prop_sizer->Add( m_ylabel, 0,  wxALL|wxEXPAND, 1 );
	
	prop_sizer->Add( new wxStaticText( this, wxID_ANY, "Size:" ),0, wxALL|wxALIGN_CENTER_VERTICAL, 2 );
	prop_sizer->Add( m_size, 0, wxALL|wxEXPAND, 1 );

	prop_sizer->Add( new wxStaticText( this, wxID_ANY, "Text:" ), 0, wxALL|wxALIGN_CENTER_VERTICAL, 2 );
	
	wxBoxSizer *text_sizer = new wxBoxSizer( wxHORIZONTAL );
	text_sizer->Add( m_scale, 1, wxALL|wxEXPAND, 1 );
	text_sizer->Add( m_font, 0, wxALL, 1 );
	prop_sizer->Add( text_sizer, 0, wxALL|wxEXPAND, 1 );

	prop_sizer->Add( m_showLegend, 0, wxALL|wxALIGN_CENTER_VERTICAL, 4 );
	prop_sizer->Add( m_legendPos, 0, wxALL, 1 );


	wxBoxSizer *chk_sizer = new wxBoxSizer( wxHORIZONTAL );
	chk_sizer->Add( m_coarse, 1, wxALL|wxEXPAND, 4 );
	chk_sizer->Add( m_fine, 1, wxALL|wxEXPAND, 4 );
		

	wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
	sizer->Add( m_type, 0, wxALL|wxEXPAND, 4 );
	sizer->Add( prop_sizer, 0, wxALL|wxEXPAND, 4 );
	sizer->Add( chk_sizer, 0, wxALL|wxEXPAND, 4 );
	sizer->Add(m_srch, 0,  wxEXPAND|wxALL, 1);
	sizer->Add(m_Y, 1, wxALL | wxEXPAND, 0);
	SetSizer(sizer);
	
	Enable( false );
}


void GraphProperties::SetupVariables( Simulation *sim )
{

	Clear();

	if ( !sim ) return;
	m_sim = sim;
	m_names.Clear();
	m_selected.Clear();
	m_srch->Clear();

	int vsx, vsy;
	m_Y->GetViewStart( &vsx, &vsy );	
	m_Y->Freeze();
	m_Y->RemoveAll();

	PopulateSelectionList( m_Y, &m_names, sim );

	m_Y->ExpandSelections();
	m_Y->Scroll( vsx, vsy );
	m_Y->Thaw();


	Enable( true );
}

void GraphProperties::Clear()
{
	m_xlabel->Clear();
	m_ylabel->Clear();
	m_title->Clear();

	Enable( false );
}


void GraphProperties::Set( const Graph &g )
{

	m_names.Clear();
	m_selected.Clear();
	m_srch->Clear();

	int vsx, vsy;
	m_Y->GetViewStart(&vsx, &vsy);
	m_Y->Freeze();
	m_Y->RemoveAll();

	PopulateSelectionList(m_Y, &m_names, m_sim);

	m_Y->ExpandSelections();
	m_Y->Scroll(vsx, vsy);
	m_Y->Thaw();


	for( size_t i=0;i<m_names.size();i++ )
		m_Y->SelectRowInCol( i, 0, g.Y.Index( m_names[i] ) != wxNOT_FOUND );

	m_type->SetSelection( g.Type );
	m_title->ChangeValue( g.Title );
	m_xlabel->ChangeValue( g.XLabel );
	m_ylabel->ChangeValue( g.YLabel );
	m_scale->SetValue( (int)(g.FontScale*10) );
	m_font->SetSelection( g.FontFace );
	m_size->SetValue( g.Size );
	m_coarse->SetValue( g.CoarseGrid );
	m_fine->SetValue( g.FineGrid );
	m_showLegend->SetValue( g.ShowLegend );
	m_legendPos->SetSelection( g.LegendPos );

	Enable( true );
	m_legendPos->Enable( m_showLegend->GetValue() );
	m_Y->ExpandSelections();
}

void GraphProperties::Get( Graph &g )
{
	for( size_t i=0;i<m_names.size();i++)
	{
		if ( g.Y.Index( m_names[i] ) != wxNOT_FOUND && !m_Y->IsRowSelected( i, 0 ) )
			g.Y.Remove( m_names[i] );

		if ( m_Y->IsRowSelected( i, 0 ) && g.Y.Index( m_names[i] ) == wxNOT_FOUND )
			g.Y.Add( m_names[i] );
	}

	g.Type = m_type->GetSelection();
	g.Title = m_title->GetValue();
	g.XLabel = m_xlabel->GetValue();
	g.YLabel = m_ylabel->GetValue();
	g.FontScale = ((double)m_scale->GetValue())/10.0;
	g.FontFace = m_font->GetSelection();
	g.Size = m_size->GetValue();
	g.CoarseGrid = m_coarse->GetValue();
	g.FineGrid = m_fine->GetValue();
	g.ShowLegend = m_showLegend->GetValue();
	g.LegendPos = m_legendPos->GetSelection();
}

void GraphProperties::SendChangeEvent()
{
	wxCommandEvent e( wxEVT_GRAPH_PROPERTY_CHANGE, GetId() );
	e.SetEventObject( this );
	GetEventHandler()->ProcessEvent( e );
}

void GraphProperties::OnEdit(wxCommandEvent &evt)
{
	SendChangeEvent();
	m_legendPos->Enable(m_showLegend->GetValue());
}

void GraphProperties::OnSearch(wxCommandEvent &evt)
{
	m_Y->Filter( m_srch->GetValue() );
	m_Y->ExpandAll();
}

void GraphProperties::OnSlider( wxScrollEvent & )
{
	SendChangeEvent();
}


enum { ID_CREATE_GRAPH = wxID_HIGHEST+466, ID_DELETE_GRAPH,
	ID_GRAPH_PROPS };

BEGIN_EVENT_TABLE(GraphViewer, wxPanel)
	EVT_BUTTON( ID_CREATE_GRAPH, GraphViewer::OnCommand )
	EVT_BUTTON( ID_DELETE_GRAPH, GraphViewer::OnCommand )
	EVT_GRAPH_PROPERTY_CHANGE( ID_GRAPH_PROPS, GraphViewer::OnCommand )
	EVT_GRAPH_SELECT( wxID_ANY, GraphViewer::OnGraphSelect )
END_EVENT_TABLE()


GraphViewer::GraphViewer(wxWindow *parent) : wxPanel(parent, wxID_ANY)
//	: wxSplitterWindow( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE|wxSP_NOBORDER )
{
	m_sim = 0;
	m_current = 0;

	wxBoxSizer *main_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxSplitterWindow *splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE | wxSP_NOBORDER | wxSP_3DSASH );
	main_sizer->Add(splitter, 1, wxBOTTOM | wxLEFT | wxEXPAND, 0);

	m_lpanel = new wxPanel(splitter);
	wxBoxSizer *sizer_tools = new wxBoxSizer( wxHORIZONTAL );
	sizer_tools->Add( new wxMetroButton( m_lpanel, ID_CREATE_GRAPH, "Create graph" ), 1, wxALL|wxEXPAND, 0 );
	sizer_tools->Add( m_delButton = new wxMetroButton( m_lpanel, ID_DELETE_GRAPH, "Delete" ), 0, wxALL|wxEXPAND, 0 );
		
	m_props = new GraphProperties( m_lpanel, ID_GRAPH_PROPS );

	wxBoxSizer *sizer_left = new wxBoxSizer( wxVERTICAL );
	sizer_left->Add( sizer_tools, 0, wxALL|wxEXPAND, 0 );
	sizer_left->Add( m_props, 1, wxALL|wxEXPAND, 0 );


	m_lpanel->SetSizer( sizer_left );

	m_layout = new wxSnapLayout(splitter, wxID_ANY);
	m_layout->SetShowSizing( true );

	splitter->SetMinimumPaneSize( 50 );
	splitter->SplitVertically( m_lpanel, m_layout, (int)(260*wxGetScreenHDScale()) );

	SetSizer(main_sizer);
	main_sizer->SetSizeHints(this);
	
	UpdateProperties();
}

GraphCtrl *GraphViewer::CreateNewGraph()
{
	GraphCtrl *gc = new GraphCtrl( m_layout, wxID_ANY );
	m_graphs.push_back( gc );
	m_layout->Add( gc );
	return gc;
}

void GraphViewer::DeleteGraph( GraphCtrl *gc )
{
	std::vector<GraphCtrl*>::iterator it = std::find( m_graphs.begin(), m_graphs.end(), gc );
	if ( it != m_graphs.end() )
	{
		if ( m_current == *it )
			m_current = 0;

		m_layout->Delete( *it );
		m_graphs.erase( it );
	}
}

void GraphViewer::DeleteAll()
{
	for( std::vector<GraphCtrl*>::iterator it = m_graphs.begin();
		it != m_graphs.end();
		++it )
	{
		m_layout->Delete( *it );
		m_graphs.erase( it );
	}

	m_current = 0;
}

void GraphViewer::SetGraphs( std::vector<Graph> &gl )
{
	DeleteAll();
	
	for( size_t i=0;i<gl.size();i++ )
	{
		GraphCtrl *gc = CreateNewGraph();
		gc->SetGraph(gl[i]);
	}
}

void GraphViewer::GetGraphs( std::vector<Graph> &gl )
{
	gl.clear();
	gl.reserve( m_graphs.size() );
	for( size_t i=0;i<m_graphs.size();i++ )
		gl.push_back( m_graphs[i]->GetGraph() );
}

	
void GraphViewer::Setup( Simulation *sim )
{
	m_sim = sim;
	m_props->Clear();

	if ( !m_sim ) return;

	m_props->SetupVariables( m_sim );

	for( std::vector<GraphCtrl*>::iterator it = m_graphs.begin();
		it != m_graphs.end();
		++it )
	{
		Graph g = (*it)->GetGraph();
		(*it)->Display( m_sim, g );
	}

	if ( m_current != 0 )
		m_props->Set( m_current->GetGraph() );
}


GraphCtrl *GraphViewer::Current()
{
	return m_current;
}

void GraphViewer::UpdateGraph()
{
	if( !m_current || !m_sim) return;
	Graph g = m_current->GetGraph();
	m_props->Get( g );
	m_current->Display( m_sim, g );
}

void GraphViewer::UpdateProperties()
{
	if ( m_current ) {
		m_props->Set( m_current->GetGraph() );
		m_props->Show();
		m_delButton->Show();
		m_layout->SetBackgroundText( wxEmptyString );
	}
	else {
		m_props->Clear();
		m_props->Hide();
		m_delButton->Hide();
		m_layout->SetBackgroundText( "Click Create Graph to begin" );
	}

	Layout();
	m_lpanel->Layout();
}

void GraphViewer::OnCommand( wxCommandEvent &evt )
{
	if ( evt.GetId() == ID_CREATE_GRAPH )
	{
		SetCurrent( CreateNewGraph() );
	}
	else if ( evt.GetId() == ID_DELETE_GRAPH )
	{
		if ( m_current != 0 )
		{
			DeleteGraph( m_current );
			m_current = 0;
			SetCurrent( 0 );
		}
	}
	else if ( evt.GetId() == ID_GRAPH_PROPS )
		UpdateGraph();
}

void GraphViewer::OnGraphSelect( wxCommandEvent &evt )
{
	if ( GraphCtrl *gc = dynamic_cast<GraphCtrl*>( evt.GetEventObject() ) )
		SetCurrent( gc );
}

void GraphViewer::SetCurrent( GraphCtrl *gc )
{
	if ( m_current )
	{
		m_layout->ClearHighlights();
		m_current = 0;
	}
	
	m_current = gc;

	if(  m_current )
		m_layout->Highlight( m_current );

	UpdateProperties();
}

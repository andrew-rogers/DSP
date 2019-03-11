/**
 *
 * @licstart  The following is the entire license notice for the 
 *  JavaScript code in this page.
 *
 * Copyright (C) 2015  Andrew Rogers
 *
 *
 * The JavaScript code in this page is free software: you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License (GNU GPL) as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.  The code is distributed WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU GPL for more details.
 *
 * As additional permission under GNU GPL version 3 section 7, you
 * may distribute non-source (e.g., minimized or compacted) forms of
 * that code without the copy of the GNU GPL normally required by
 * section 4, provided you include this license notice and a URL
 * through which recipients can access the Corresponding Source.
 *
 * @licend  The above is the entire license notice
 * for the JavaScript code in this page.
 *
 */

var Table=function(el,filter) {
  // If width is zero then set to sensible value
  if(el.width()<=0)el.width(400);
  el.css('clear','both');
  
  this.el=el;
  var div_butt=$('<div></div>');
  this.div_tab=$('<div></div>');
  el.html(this.div_tab).append(div_butt);
  var butt_addbq=$('<button>Add another biquadratic section</button>');
  var butt_update=$('<button>Update plots</button>');
  div_butt.html(butt_addbq).append(butt_update);

  var $this=$(this);
  var that=this;

  butt_addbq.click( function() {
    filter.zeros.push([0,0]);
    filter.poles.push([0,0]);
    that.updateTable();
    $this.trigger('changed');
  });

  butt_update.click( function() {
    that.readTable();
    $this.trigger('changed');
  });

  this.filter=filter;

  this.updateTable();
};

Table.prototype.addRow=function() {
};

Table.prototype.removeRow=function(bq_id) {
  this.filter.zeros.splice(bq_id,1);
  this.filter.poles.splice(bq_id,1);
  this.updateTable();
  $(this).trigger('changed');
};

Table.prototype.readTable=function ( filter ) {
  var nr=this.gl.cells.length;
  for(var r=1; r<nr; r++){
    var zr=this.gl.cells[r][0].children('input').val();
    var za=this.gl.cells[r][1].children('input').val();
    var pr=this.gl.cells[r][2].children('input').val();
    var pa=this.gl.cells[r][3].children('input').val();   
    var z=rcmul(zr,expi(za));
    var p=rcmul(pr,expi(pa));
    this.filter.zeros[r-1]=z;
    this.filter.poles[r-1]=p;
  }
};

Table.prototype.updateTable=function() {
  var nr=this.filter.zeros.length+1;
  this.div_tab.height(nr*20);
  this.div_tab.width(500);
  this.gl=new GridLayout(this.div_tab,nr,[50,50,50,50,20]);

  // Table header
  this.gl.cells[0][0].html("Zero rad.");
  this.gl.cells[0][1].html("Zero arg.");
  this.gl.cells[0][2].html("Pole rad.");
  this.gl.cells[0][3].html("Pole arg.");
  this.gl.cells[0][4].html("Remove");

  for(var i=1; i<nr; i++){
    this._updateCell(i,0,this.filter.zeros[i-1]);
    this._updateCell(i,2,this.filter.poles[i-1]);
    var butt_remove=new CanvasButton(this.gl.cells[i][4],'control.delete');
    butt_remove.bq_id=i-1;
    var that=this;
    $(butt_remove).click( function() {
      that.removeRow(this.bq_id);
    });
  }
};

Table.prototype._updateCell=function(ri,ci,v) {
  var rad=Math.sqrt(v[0]*v[0]+v[1]*v[1]);
  var arg=Math.atan2(v[1],v[0]);
  var in_rad=$('<input type="text" size="10" id="'+ri+'" value="'+rad+'">');
  var in_arg=$('<input type="text" size="10" id="'+ri+'" value="'+arg+'">');
  this.gl.cells[ri][ci].html(in_rad);
  this.gl.cells[ri][ci+1].html(in_arg);
  var $this=$(this);
  in_rad.focus(function() {
    $this.trigger('selected',[this.id-1]);
  });
  in_arg.focus(function() {
    $this.trigger('selected',[this.id-1]);
  });
};

var Spinner=function(el,filter) {
  el.width(400);
  el.css('clear','both');
  var div_zspin=$('<div style="width: 50%; float: left;"></div>');
  var div_pspin=$('<div style="width: 50%; float: left;"></div>');
  el.html(div_zspin).append(div_pspin);
  this.zspin=new CoordSpinner(div_zspin);
  this.pspin=new CoordSpinner(div_pspin);
  this.zspin.setLabels("arg: ","rad: ");
  this.pspin.setLabels("arg: ","rad: ");
  
  var that=this;
  var $this=$(this);
  $(this.zspin).on('changed', function(e,za,zr) {
    filter.setZeroFromPolar(that.selected_bq_id,[zr,za]);
    $this.trigger('changed',[that.selected_bq_id]);
  });
  $(this.pspin).on('changed', function(e,pa,pr) {
    filter.setPoleFromPolar(that.selected_bq_id,[pr,pa]);
    $this.trigger('changed',[that.selected_bq_id]);
  });
  
  this.filter=filter;
}

Spinner.prototype.select=function(bq_id) {
  var polar=this.filter.getPolar(bq_id);
  this.zspin.setValue(polar[0][1],polar[0][0]);
  this.pspin.setValue(polar[1][1],polar[1][0]);
  this.selected_bq_id=bq_id;
};


$(document).ready(function(){
  // Create a 4th order elliptic low-pass IIR filter
  var filter=new Filter();
  filter.elliptic_lpf();

  // Create input table and buttons
  var spinner=new Spinner($('#spinner'),filter);
  var tab=new Table($('#user_input'),filter);
  
  // Plot the zeros and poles on zplane
  var plot_zp = $.jqplot ('zpplot', filter.getZP(),{
    sortData : false,
    seriesDefaults:{
      renderer : $.jqplot.ZplaneRenderer,
      isDragable : true
    },
    axes:{
      xaxis:{ min : -1.1, max : 1.1 },
      yaxis:{ min : -1.1, max : 1.1 }
    }
  });
  
  // Plot frequency response
  var plot_fr = $.jqplot ('frplot', [filter.getFreqResponse()],{
    seriesDefaults:{
      showMarker : false
    }   
  });

  var replot_zp = function(){
    var zp=filter.getZP();
    plot_zp.series[0].data=zp[0];
    plot_zp.series[1].data=zp[1];
    plot_zp.replot();
  };

  var replot_fr = function(){
    var fr=filter.getFreqResponse();
    plot_fr.series[0].data=fr;
    plot_fr.replot();
  };
  
  $(spinner).on('changed', function() {
    tab.updateTable();
    replot_zp();
    replot_fr();
  });

  $(tab).on('changed', function() {
    replot_zp();
    replot_fr();
  });

  $(tab).on('selected', function(e,bq_id) {
    console.log(">"+bq_id);
    spinner.select(bq_id);
  });

  $('#zpplot').bind('jqplotSeriesPointChange', function (e, seriesIndex, pointIndex, val) {
    if(pointIndex%2==1){
      // Conjugate is being dragged
      pointIndex=pointIndex-1;
      val[1]=-val[1];
    }
    if(seriesIndex==0){
      filter.zeros[pointIndex/2]=val;
    }
    else if(seriesIndex==1){
      filter.poles[pointIndex/2]=val;
    }
    replot_fr();
    tab.updateTable();
  });

  $('#zpplot').bind('jqplotDragStop', function (e, pixelPos, val) {
    // Replot the zero-pole plot to correct conjugates
    replot_zp();
  });  
});

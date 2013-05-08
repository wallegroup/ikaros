function FourBars(p)
{
	this.inited = false;
	this.graph = new Graph(p, p.module+'.'+p.source);
	this.module = p.module;
	this.source = p.source;
	this.width = p.width;
	this.height = p.height;
	this.decimals = (p.decimals == null) ? 2 : p.decimals;
	this.LUTp = makeLUTArray((p.positive_color ? p.positive_color : p.color), ['#dAdAdA']);
	this.LUTn = makeLUTArray((p.negative_color ? p.negative_color : p.color), ['#dA6A6A']);
	this.font_size = p.font_size;

	usesData(this.module, this.source);
}



FourBars.prototype.Init = function(data)
{
	var d = data[this.module][this.source];

	this.inited = true;    
	this.sizey = d.length;
	this.sizex = d[0].length;	
	var box_width = (this.width-2)/this.sizex;
	var box_height = (this.height-2)/this.sizey;
	this.box = new Array(this.sizey);
	var cc = 0;
	
	this.box = new Array(this.sizex);
	this.box[0] = this.graph.AddText(this.width*0.5, this.height*0.25, '0', box_width, box_height, 'end', this.font_size, this.LUTp[cc++ % this.LUTp.length]);
	this.box[1] = this.graph.AddText(this.width*0.75, this.height*0.5, '0', box_width, box_height, 'end', this.font_size, this.LUTp[cc++ % this.LUTp.length]);
	this.box[2] = this.graph.AddText(this.width*0.5, this.height*0.75, '0', box_width, box_height, 'end', this.font_size, this.LUTp[cc++ % this.LUTp.length]);
	this.box[3] = this.graph.AddText(this.width*0.25, this.height*0.5, '0', box_width, box_height, 'end', this.font_size, this.LUTp[cc++ % this.LUTp.length]);
}



FourBars.prototype.Update = function(data)
{
	var d = data[this.module];
	if(!d) return;
	d = d[this.source]
	if(!d) return;

	if(!this.inited) this.Init(data);

	var cc = 0;
	for(var i=0; i<this.sizex; i++)
	{
		this.box[i].firstChild.nodeValue = d[0][i].toFixed(this.decimals);
		this.box[i].setAttribute('fill', (d[0][i] >= 0 ? this.LUTp[cc++ % this.LUTp.length] : this.LUTn[cc++ % this.LUTp.length]));
	}
}

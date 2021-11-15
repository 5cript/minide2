import minide from 'minide';

class CMakeToolbar extends minide.Toolbar
{	
	name = "CMake C++";
	id = "cmake_toolbar";

	constructor() 
	{
		super();
		this.editor = new minide.EditorControl;
		this.resources = new minide.ResourceAccessor;
	}
	
	initialize = (...args) =>
	{
		return [
			this.makeMenu([
				{
					label: "Save",
					pngbase64: this.resources.loadPng("./images/save.png"),
					action: () => {
						this.editor.save();
					}
				},
				this.makeSplitter(),
				{
					label: "Project Settings",
					action: () => {
						console.log('ProjectSettingsActionCalled')
					}
				},
				{
					label: "Add New Target",
					action: () => {
						console.log("NewTargetCalled")
					}
				}
			]),
			this.makeSplitter(),
			this.makeIconButton({
				id: "save",
				action: () => {
					this.editor.save();
				},
				pngbase64: ""
			}),
			this.makeIconButton({
				id: "saveAll",
				action: () => {
					this.editor.saveAll();
				},
				pngbase64: ""
			})
		]
	}
};

export default CMakeToolbar;
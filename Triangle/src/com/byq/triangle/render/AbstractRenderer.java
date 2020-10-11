package com.byq.triangle.render;

abstract public class AbstractRenderer implements Renderer {
	public AbstractRenderer(TranslatorSource trans, NetSource ns) {
		this.translatorSource = trans;
		this.netSource = ns;

	}

	TranslatorSource translatorSource;

	NetSource netSource;

	RenderWindow renderWindow;

	public TranslatorSource getTranslatorSource() {
		return translatorSource;
	}

	public NetSource getNetSource() {
		return netSource;
	}

	public RenderWindow getRenderWindow() {
		return renderWindow;
	}

}

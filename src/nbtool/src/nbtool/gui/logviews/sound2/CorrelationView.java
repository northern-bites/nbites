package nbtool.gui.logviews.sound2;

import nbtool.data.log._Log;
import nbtool.gui.logviews.misc.ViewParent;

public class CorrelationView extends ViewParent {
	
	private static final int TARGET_F = 2000;
	
	public CorrelationView() {
		super();
		initComponents();
		
		magSlider.setEnabled(false);
		diffSlider.setEnabled(false);
		
//		waveScrollPane.setVisible(false);
	}
	
	private int percentForRange(double min, double max, double val) {
		assert( min < max && val >= min && val <= max);
		double fraction = (val - min) / (max - min);
		return (int) (100 * fraction);
	}
	
	private double used(short val) {
		return ( (double) Math.abs(val) ) / Short.MAX_VALUE;
	}

	@Override
	public void setLog(_Log newlog) {
		
		int srate = newlog.sexprForContentItem(0)
				.firstValueOf("rate").valueAsInt();
		
		//nyquist check
		assert(srate > (TARGET_F * 2));
		
		ShortBuffer buf = new ShortBuffer();
		buf.parse(newlog);
		Correlator cor = new Correlator(buf.channels,
				srate / TARGET_F, 1.0d);
		
		String useText = String.format("formate usage: left [%.3f, %.3f] -- right [%.3f, %.3f]",
				used(buf.min[0]), used(buf.max[0]), used(buf.min[1]), used(buf.max[1]) );
		formatUsageStats.setText(useText);
		
//		System.out.printf("%d %d %.3f %.3f\n",
//				buf.min[0], buf.max[0], used(buf.min[0]), used(buf.max[0]) );
				
		
		details1.setText(String.format("srate=%d target_f=%d", 
				srate, TARGET_F));
		details2.setText(String.format("frames=%d chnls=%d", 
				buf.frames, buf.channels));
		
		for (int j = 0; j < buf.channels; ++j) {
			for (int i = 0; i < buf.frames; ++i) {
				cor.correlate(j, i, buf.get(j, i));
			}
		}
		
		double lo = cor.offset(0);
		double ro = cor.offset(1);
		
		leftLabel.setText("left " + lo);
		leftBar.setValue(
				percentForRange(-Math.PI, Math.PI, lo)
				);
		
		rightLabel.setText("right " + ro);
		rightBar.setValue(
				percentForRange(-Math.PI, Math.PI, ro)
				);
		
		double diff = lo - ro;
		diffLabel.setText("difference " + diff);
		diffSlider.setValue(
				percentForRange(-2 *Math.PI, 2 * Math.PI, diff)
				);
		
		double lm = cor.magnitude(0);
		double rm = cor.magnitude(1);
		magLabel.setText(String.format("magnitude: (%s, %s)",
				String.format("%6.3e", lm), String.format("%6.3e", rm)));
		int ms = (int) (100 * (lm / (lm + rm)));
		magSlider.setValue(ms);
	}
	
	private void showFirstBoxActionPerformed(java.awt.event.ActionEvent evt) {                                             
        // TODO add your handling code here:
    }                                            

    private void showTargetBoxActionPerformed(java.awt.event.ActionEvent evt) {                                              
        // TODO add your handling code here:
    }                                             

    private void showSecondBoxActionPerformed(java.awt.event.ActionEvent evt) {                                              
        // TODO add your handling code here:
    } 
	
	
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">                          
    private void initComponents() {

        leftBar = new javax.swing.JProgressBar();
        rightBar = new javax.swing.JProgressBar();
        leftLabel = new javax.swing.JLabel();
        rightLabel = new javax.swing.JLabel();
        diffLabel = new javax.swing.JLabel();
        magLabel = new javax.swing.JLabel();
        DMIN_LABEL = new javax.swing.JLabel();
        DMAX_LABEL = new javax.swing.JLabel();
        magSlider = new javax.swing.JSlider();
        diffSlider = new javax.swing.JSlider();
        details1 = new javax.swing.JLabel();
        details2 = new javax.swing.JLabel();
        formatUsageStats = new javax.swing.JLabel();
        waveScrollPane = new javax.swing.JScrollPane();
        showTargetBox = new javax.swing.JCheckBox();
        showFirstBox = new javax.swing.JCheckBox();
        showSecondBox = new javax.swing.JCheckBox();

        leftLabel.setText("left");

        rightLabel.setText("right");

        diffLabel.setText("difference");

        magLabel.setText("magnitude:");

        DMIN_LABEL.setText("-2PI");

        DMAX_LABEL.setText("2PI");

        magSlider.setToolTipText("");
        magSlider.setFocusable(false);

        diffSlider.setMinimum(-100);
        diffSlider.setFocusable(false);

        details1.setText("jLabel1");

        details2.setText("jLabel2");

        formatUsageStats.setText("jLabel1");

        showTargetBox.setText("target");
        showTargetBox.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                showTargetBoxActionPerformed(evt);
            }
        });

        showFirstBox.setText("first");
        showFirstBox.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                showFirstBoxActionPerformed(evt);
            }
        });

        showSecondBox.setText("second");
        showSecondBox.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                showSecondBoxActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(waveScrollPane)
                    .addComponent(magLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(magSlider, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(diffSlider, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                        .addComponent(DMIN_LABEL)
                        .addGap(206, 206, 206)
                        .addComponent(diffLabel, javax.swing.GroupLayout.PREFERRED_SIZE, 236, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 344, Short.MAX_VALUE)
                        .addComponent(DMAX_LABEL))
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                            .addComponent(leftLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(rightLabel, javax.swing.GroupLayout.DEFAULT_SIZE, 223, Short.MAX_VALUE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(rightBar, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(leftBar, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)))
                    .addGroup(layout.createSequentialGroup()
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                            .addComponent(details1, javax.swing.GroupLayout.DEFAULT_SIZE, 345, Short.MAX_VALUE)
                            .addComponent(details2, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addGroup(layout.createSequentialGroup()
                                .addComponent(showTargetBox)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                .addComponent(showFirstBox)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                .addComponent(showSecondBox)
                                .addGap(0, 0, Short.MAX_VALUE))
                            .addComponent(formatUsageStats, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(details1)
                    .addComponent(formatUsageStats))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(details2)
                    .addComponent(showTargetBox)
                    .addComponent(showFirstBox)
                    .addComponent(showSecondBox))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(waveScrollPane, javax.swing.GroupLayout.PREFERRED_SIZE, 288, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(leftBar, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(leftLabel))
                .addGap(5, 5, 5)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(rightBar, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(rightLabel))
                .addGap(15, 15, 15)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(diffLabel)
                    .addComponent(DMIN_LABEL)
                    .addComponent(DMAX_LABEL))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(diffSlider, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(15, 15, 15)
                .addComponent(magLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(magSlider, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(15, 15, 15))
        );
    }// </editor-fold>                        

    // Variables declaration - do not modify                     
    private javax.swing.JLabel DMAX_LABEL;
    private javax.swing.JLabel DMIN_LABEL;
    private javax.swing.JLabel details1;
    private javax.swing.JLabel details2;
    private javax.swing.JLabel diffLabel;
    private javax.swing.JSlider diffSlider;
    private javax.swing.JLabel formatUsageStats;
    private javax.swing.JProgressBar leftBar;
    private javax.swing.JLabel leftLabel;
    private javax.swing.JLabel magLabel;
    private javax.swing.JSlider magSlider;
    private javax.swing.JProgressBar rightBar;
    private javax.swing.JLabel rightLabel;
    private javax.swing.JCheckBox showFirstBox;
    private javax.swing.JCheckBox showSecondBox;
    private javax.swing.JCheckBox showTargetBox;
    private javax.swing.JScrollPane waveScrollPane;
    // End of variables declaration   
}

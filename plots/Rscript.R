library(ggplot2)
library(grid)
R4_w1 <- read.csv(file.choose(), header = FALSE, '\t')
R4_w2 <- read.csv(file.choose(), header = FALSE, '\t')
R4_w7 <- read.csv(file.choose(), header = FALSE, '\t')
R4_w10 <- read.csv(file.choose(), header = FALSE, '\t')

p <- ggplot(data = R4_w1, aes(x = R4_w1$V1, y = R4_w1$V2)) + labs(color = "") + scale_colour_manual(breaks=c("Weights Ratio = 10","Weights Ratio = 7","Weights Ratio = 2", "Weights Ratio = 1"), labels=c("Weights Ratio = 10", "Weights Ratio = 7", "Weights Ratio = 2", "Weights Ratio = 1"), values=c("#F8766D", "#7CAE00", "#00BFC4", "#C77CFF")) + scale_x_continuous(expand = c(0, 0)) + scale_y_continuous(breaks=c(1,2,7,10)) + xlab("\nTime (s)\n") + ylab("\nPerceived Bandwidths Ratio\n") + theme(axis.text = element_text(colour = "black"), legend.title=element_blank(), legend.background = element_rect(colour = "black"), legend.key = element_rect(fill = "white"), legend.key.size = unit(1.5, "cm"), panel.grid.major = element_blank(), panel.grid.minor = element_blank(), panel.background = element_blank(), axis.line = element_line(colour = "black"), text = element_text(size=35))+ guides(colour = guide_legend(override.aes = list(size=3))) + expand_limits(x = 0)
p <- p + geom_line(data = R4_w1, aes(x = R4_w1$V1, y = R4_w1$V2, colour="Weights Ratio = 1"))
p <- p + geom_line(data = R4_w2, aes(x = R4_w2$V1, y = R4_w2$V2, colour="Weights Ratio = 2"))
p <- p + geom_line(data = R4_w7, aes(x = R4_w7$V1, y = R4_w7$V2, colour="Weights Ratio = 7"))
p <- p + geom_line(data = R4_w10, aes(x = R4_w10$V1, y = R4_w10$V2, colour="Weights Ratio = 10"))
p